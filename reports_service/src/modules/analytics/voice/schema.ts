
// analyticsReportScheme
export const analyticsReportScheme = {
  body: {
    type: 'object',
    properties: {
      startTmpstmp: { type: 'number' },
      endTmpstmp: { type: 'number' },
      timeZone: { type: 'string' },
      domainId: { type: 'number' },
      p_Limit: { type: 'number' },
      p_offset: { type: 'number' }
    },
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
      },
    },
  },
};

// agentInboundCallAnalysisMetricsFilterScheme
export const agentInboundCallAnalysisMetricsFilterScheme = {
  body: {
    type: 'object',
    properties: {
      startTimeStamp: { type: ['number', 'null'] },
      endTimeStamp: { type: ['number', 'null'] },
      offset: { type: 'number' },
      limit: { type: 'number' },
      UserName: { type: ['string', 'null'] }
    },
    required: ['offset', 'limit', 'startTimeStamp', 'endTimeStamp']
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        AgentDetails: {
          type: 'array',
          items: {
            type: 'object',
            properties: {
              UserID: { type: 'number' },
              UserName: { type: 'string' }
            },
            required: ['UserID', 'UserName']
          }
        },
        AllAgentInboundCall: {
          type: 'array',
          items: {
            type: 'object',
            properties: {
              TotalCount: { type: 'number' },
              DiffTotalPercent: { type: 'number' },
              CurrentAnsweredeCount: { type: 'number' },
              DiffAnsweredPercent: { type: 'number' },
              CurrentAbandonedCount: { type: 'number' },
              DiffAbandonedPercent: { type: 'number' }
            },
            required: ['TotalCount', 'DiffTotalPercent', 'CurrentAnsweredeCount', 'DiffAnsweredPercent', 'CurrentAbandonedCount', 'DiffAbandonedPercent']
          }
        },
        AgentInboundCallChart: {
          type: 'array',
          items: {
            type: 'object',
            properties: {
              agentName: { type: 'string' },
              Inboundcalls: { type: 'number' },
              AnsweredCalls: { type: 'number' },
              abandonedCalls: { type: 'number' },
              missedCalls: { type: 'number' },
              transferredCalls: { type: 'number' },
              Talktime: { type: 'number' },
              avgCallDuration: { type: 'number' },
              avgSpeedofAnswer: { type: 'number' },
              AverageHandleTime: { type: 'number' },
              maximumHandleTime: { type: 'number' },
              ACWTime: { type: 'number' },
              averageHoldTime: { type: 'number' },
              occupancyrate: { type: 'number' },
              FirstCallResolution: { type: 'number' },
              customersatisfaction: { type: 'number' }
            },
            required: ['agentName', 'Inboundcalls', 'AnsweredCalls', 'abandonedCalls', 'missedCalls', 'transferredCalls', 'Talktime', 'avgCallDuration', 'avgSpeedofAnswer', 'AverageHandleTime', 'maximumHandleTime', 'ACWTime', 'averageHoldTime', 'occupancyrate', 'FirstCallResolution', 'customersatisfaction']
          }
        }
      },
      required: ['statusCode', 'message', 'AgentDetails', 'AllAgentInboundCall', 'AgentInboundCallChart']
    }
  }
};

// agentOutboundCallAnalysisMetricsFilterScheme
export const agentOutboundCallAnalysisMetricsFilterScheme = {
  body: {
    type: 'object',
    properties: {
      startTimeStamp: { type: ['number', 'null'] },
      endTimeStamp: { type: ['number', 'null'] },
      offset: { type: 'number' },
      limit: { type: 'number' },
      UserName: { type: ['string', 'null'] }
    },
    required: ['offset', 'limit', 'startTimeStamp', 'endTimeStamp']
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        AgentDetails: {
          type: 'array',
          items: {
            type: 'object',
            properties: {
              UserID: { type: 'number' },
              UserName: { type: 'string' }
            },
            required: ['UserID', 'UserName']
          }
        },
        AllAgentOutboundCall: {
          type: 'array',
          items: {
            type: 'object',
            properties: {
              TotalCount: { type: 'number' },
              DiffTotalPercent: { type: 'number' },
              CurrentAnsweredeCount: { type: 'number' },
              DiffAnsweredPercent: { type: 'number' },
              CurrentUnAnsweredCount: { type: 'number' },
              DiffUnAnsweredPercent: { type: 'number' }
            },
            required: ['TotalCount', 'DiffTotalPercent', 'CurrentAnsweredeCount', 'DiffAnsweredPercent', 'CurrentUnAnsweredCount', 'DiffUnAnsweredPercent']
          }
        },
        AgentOutboundCallChart: {
          type: 'array',
          items: {
            type: 'object',
            properties: {
              agentName: { type: 'string' },
              Outboundcalls: { type: 'number' },
              AnsweredCalls: { type: 'number' },
              UnAnsweredCalls: { type: 'number' },
              abandonedCalls: { type: 'number' },
              transferredCalls: { type: 'number' },
              Talktime: { type: 'number' },
              avgCallDuration: { type: ['number', 'null'] },
              avgSpeedofAnswer: { type: 'string' },
              AverageHandleTime: { type: 'number' },
              maximumHandleTime: { type: 'number' },
              ACW: { type: 'number' },
              averageHoldTime: { type: 'number' },
              occupancyrate: { type: ['number', 'null'] },
              customersatisfaction: { type: 'number' },
              FirstCallResolution: { type: 'number' }
            },
            required: ['agentName', 'Outboundcalls', 'AnsweredCalls', 'UnAnsweredCalls', 'abandonedCalls', 'transferredCalls', 'Talktime', 'avgCallDuration', 'avgSpeedofAnswer', 'AverageHandleTime', 'maximumHandleTime', 'ACW', 'averageHoldTime', 'occupancyrate', 'customersatisfaction', 'FirstCallResolution']
          }
        }
      },
      required: ['statusCode', 'message', 'AgentDetails', 'AllAgentOutboundCall', 'AgentOutboundCallChart']
    }
  }
};

// agentCallbackAnalysisMetricsFilterScheme
export const agentCallbackAnalysisMetricsFilterScheme = {
  body: {
    type: 'object',
    properties: {
      startTimeStamp: { type: ['number', 'null'] },
      endTimeStamp: { type: ['number', 'null'] },
      offset: { type: 'number' },
      limit: { type: 'number' },
      UserName: { type: ['string', 'null'] }
    },
    required: ['offset', 'limit', 'startTimeStamp', 'endTimeStamp']
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        AgentDetails: {
          type: 'array',
          items: {
            type: 'object',
            properties: {
              UserID: { type: 'number' },
              UserName: { type: 'string' },
              ext: { type: ['number', 'null'] } // Added ext property to AgentDetails
            },
            required: ['UserID', 'UserName', 'ext']
          }
        },
        AllagentCallBack: {
          type: 'array',
          items: {
            type: 'object',
            properties: {
              TotalCount: { type: 'number' },
              DiffTotalPercent: { type: 'number' },
              TotalCallBackReturend: { type: ['number', 'null'] },
              DiffCallBackReturend: { type: 'number' },
              TotalcallBackUnanswered: { type: ['number', 'null'] },
              DiffCallBackUnanswered: { type: 'number' },
            },
            required: ['TotalCount', 'DiffTotalPercent', 'TotalCallBackReturend', 'DiffCallBackReturend', 'TotalcallBackUnanswered', 'DiffCallBackUnanswered']
          }
        },
        AgentCallBackChart: {
          type: 'array',
          items: {
            type: 'object',
            properties: {
              agentName: { type: 'string' },
              callBackReceived: { type: 'number' },
              callBackReturend: { type: ['number', 'null'] },
              callBackAnswered: { type: ['number', 'null'] },
              callBackUnanswered: { type: ['number', 'null'] },
              abandonedCall: { type: 'number' },
              callBackDuration: { type: ['number', 'null'] },
              avgHandlingTime: { type: ['number', 'null'] },
              maxHandlingTime: { type: ['number', 'null'] },
              overallConversationRate: { type: ['number', 'null'] },
              callbackSuccessRate: { type: ['number', 'null'] }
            },
            required: ['agentName', 'callBackReceived', 'callBackReturend', 'callBackAnswered', 'callBackUnanswered', 'abandonedCall', 'callBackDuration', 'avgHandlingTime', 'maxHandlingTime', 'overallConversationRate', 'callbackSuccessRate']
          }
        }
      },
      required: ['statusCode', 'message', 'AgentDetails', 'AllagentCallBack', 'AgentCallBackChart']
    }
  }
};

// agentVoiceMailAnalysisMetricsFilterScheme
export const agentVoiceMailAnalysisMetricsFilterScheme = {
  body: {
    type: 'object',
    properties: {
      startTimeStamp: { type: ['number', 'null'] },
      endTimeStamp: { type: ['number', 'null'] },
      offset: { type: 'number' },
      limit: { type: 'number' },
      UserName: { type: ['string', 'null'] }
    },
    required: ['offset', 'limit', 'startTimeStamp', 'endTimeStamp']
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        AgentDetails: {
          type: 'array',
          items: {
            type: 'object',
            properties: {
              UserID: { type: 'number' },
              UserName: { type: 'string' }
            },
            required: ['UserID', 'UserName']
          }
        },
        AllAgentVoiceMail: {
          type: 'array',
          items: {
            type: 'object',
            properties: {
              TotalCount: { type: 'number' },
              DiffTotalPercent: { type: 'number' },
              CurrentVMAnsweredeCount: { type: 'number' },
              DiffVMAnsweredPercent: { type: 'number' },
              CurrentVMAbandonedCount: { type: 'number' },
              DiffVMAbandonedPercent: { type: 'number' }
            },
          }
        },
        AgentVoiceMailChart: {
          type: 'array',
          items: {
            type: 'object',
            properties: {
              agentName: { type: 'string' },
              VoicemailReceived: { type: 'number' },
              VoicemailResponded: { type: 'number' },
              VoicemailNotResponded: { type: 'number' },
              Total_voicemail_length: { type: 'number' },
              Avg_voicemail_length: { type: ['number', 'null'] },
            },
          }
        }
      },
    }
  }
};

// agentServiceLevelCallAnalysisMetricsFilterScheme
export const agentServiceLevelCallAnalysisMetricsFilterScheme = {
  body: {
    type: 'object',
    properties: {
      startTimeStamp: { type: ['number', 'null'] },
      endTimeStamp: { type: ['number', 'null'] },
      offset: { type: 'number' },
      limit: { type: 'number' },
      UserName: { type: ['string', 'null'] }
    },
    required: ['offset', 'limit', 'startTimeStamp', 'endTimeStamp']
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        AgentDetails: {
          type: 'array',
          items: {
            type: 'object',
            properties: {
              UserID: { type: 'number' },
              UserName: { type: 'string' }
            },
            required: ['UserID', 'UserName']
          }
        },
        AllAgentServiceLevel: {
          type: 'array',
          items: {
            type: 'object',
            properties: {
              CurrentAnsweredSLACount: { type: 'number' },
              DiffAnsweredSLAPercent: { type: 'number' },
              CurrentAbandonedSLACount: { type: 'number' },
              DiffAbandonedSLAPercent: { type: 'number' },
              CurrentSLAbreachedCount: { type: 'number' },
              DiffSLAbreachedPercent: { type: 'number' }
            },
            required: ['CurrentAnsweredSLACount', 'DiffAnsweredSLAPercent', 'CurrentAbandonedSLACount', 'DiffAbandonedSLAPercent', 'CurrentSLAbreachedCount', 'DiffSLAbreachedPercent']
          }
        },
        AgentServiceLevelChart: {
          type: 'array',
          items: {
            type: 'object',
            properties: {
              agentName: { type: 'string' },
              CallType: { type: 'string' },
              SL1: { type: 'number' },
              SL2: { type: 'number' },
              SL3: { type: 'number' },
              SL4: { type: 'number' },
              SL5: { type: 'number' },
              SL6: { type: 'number' },
              TotalSLA: { type: 'number' },
              MissedCalls: { type: 'number' },
              SLAAchieved: { type: ['number', 'null'] },
              SLAbreached: { type: ['number', 'null'] },
            },
            required: ['agentName', 'CallType', 'SL1', 'SL2', 'SL3', 'SL4', 'SL5', 'SL6', 'TotalSLA', 'MissedCalls', 'SLAAchieved', 'SLAbreached']
          }
        }
      },
    }
  }
};
