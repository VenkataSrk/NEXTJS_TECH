export const queueVoiceAnalyticsInboundMetricsFilterScheme = {
  body: {
    type: 'object',
    properties: {
      startTmpstmp: { type: ['number', 'null'] },
      endTmpstmp: { type: ['number', 'null'] },
      offset: { type: 'number' },
      limit: { type: 'number' },
    },
    required: ['offset', 'limit', 'startTmpstmp', 'endTmpstmp']
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        QueueDetails: {
          type: 'array',
          items: {
            properties: {
              qid: { type: 'number' },
              queueName: { type: 'string' },
            }
          }
        },
        QueueVoiceInboundCall: {
          type: 'array',
          items: {
            properties: {
              CurrentInboundcalls: { type: 'number' },
              DiffTotalPercent: { type: 'number' },
              CurrentAnsweredeCount: { type: 'number' },
              DiffAnsweredPercent: { type: 'number' },
              CurrentAbandonedCount: { type: 'number' },
              DiffAbandonedPercent: { type: 'number' },
            }
          }
        },
        QueueVoiceInboundTable: {
          type: 'array',
          items: {
            properties: {
              queueName: { type: 'string' },
              inboundcalls: { type: 'number' },
              answeredCalls: { type: 'number' },
              abandonedCalls: { type: 'number' },
              missedCalls: { type: 'number' },
              transferredCalls: { type: 'number' },
              talktime: { type: 'number' },
              avgSpeedofAnswer: { type: 'number' },
              averageHandleTime: { type: 'number' },
              maxiumQueueWaitTime:  { type: 'number' },
              maximumHandleTime: { type: 'number' },
              totalACWTime: { type: ['number', 'null'] },
              averageHoldTime: { type: ['number', 'null'] },
              occupancyrate: { type: ['number', 'null'] },
              firstCallResolution: { type: ['number', 'null'] },
              customersatisfaction: { type: 'number' },
              averageQueueWaitTime: { type: ['number', 'null'] },
              maximumQueueWaitTime: { type: ['number', 'null'] }
            }
          }
        }
      }
    }
  }
};

export const queueVoiceAnalyticsOutboundMetricsFilterScheme = {
  body: {
    type: 'object',
    properties: {
      startTmpstmp: { type: ['number', 'null'] },
      endTmpstmp: { type: ['number', 'null'] },
      offset: { type: 'number' },
      limit: { type: 'number' },
    },
    required: ['offset', 'limit', 'startTmpstmp', 'endTmpstmp']
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        QueueDetails: {
          type: 'array',
          items: {
            properties: {
              qid: { type: 'number' },
              queueName: { type: 'string' },
            }
          }
        },
        QueueVoiceOutboundCall: {
          type: 'array',
          items: {
            properties: {
              TotalOutboundCall: { type: 'number' },
              DiffTotalPercent: { type: 'number' },
              CurrentAnsweredeCount: { type: 'number' },
              DiffAnsweredPercent: { type: 'number' },
              CurrentUnAnsweredCount: { type: 'number' },
              DiffUnAnsweredPercent: { type: 'number' },
            }
          }
        },
        QueueVoiceOutboundTable: {
          type: 'array',
          items: {
            properties: {
              queueName: { type: 'string' },
              outboundcalls: { type: 'number' },
              answeredCalls: { type: 'number' },
              unAnsweredCalls: { type: 'number' },
              abandonedCalls: { type: 'number' },
              transferredCalls: { type: 'number' },
              talktime: { type: 'number' },
              avgSpeedofAnswer: { type: 'number' },
              averageHandleTime: { type: 'number' },
              maximumHandleTime: { type: ['number', 'null'] },
              totalACWTime: { type: ['number', 'null'] },
              averageHoldTime: { type: ['number', 'null'] },
              occupancyrate: { type: ['number', 'null'] },
              customersatisfaction: { type: 'number' },
              firstCallResolution: { type: ['number', 'null'] }
            }
          }
        }
      },
    }
  }
};

export const queueVoiceAnalyticsCallBackMetricsFilterScheme = {
  body: {
    type: 'object',
    properties: {
      startTmpstmp: { type: ['number', 'null'] },
      endTmpstmp: { type: ['number', 'null'] },
      offset: { type: 'number' },
      limit: { type: 'number' },
    },
    required: ['offset', 'limit', 'startTmpstmp', 'endTmpstmp']
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        QueueDetails: {
          type: 'array',
          items: {
            properties: {
              qid: { type: 'number' },
              queueName: { type: 'string' },
            }
          }
        },
        QueueVoiceCallBack: {
          type: 'array',
          items: {
            properties: {
              TotalCount: { type: 'number' },
              DiffTotalPercent: { type: 'number' },
              TotalCallBackReturend: { type: 'number' },
              DiffCallBackReturend: { type: 'number' },
              TotalcallBackUnanswered: { type: 'number' },
              DiffCallBackUnanswered: { type: 'number' },
            }
          }
        },
        QueueVoiceCallBackTable: {
          type: 'array',
          items: {
            properties: {
              queueName: { type: 'string' },
              callBackReceived: { type: 'number' },
              callBackReturend: { type: 'number' },
              callBackAnswered: { type: 'number' },
              callBackUnanswered: { type: 'number' },
              abandonedCallback: { type: 'number' },
              callBackDuration: { type: 'number' },
              avgHandlingTime: { type: 'number' },
              maxHandlingTime: { type: 'number' },
              overallConversationRate: { type: ['number', 'null'] },
              callbackSuccessRate: { type: ['number', 'null'] },
            }
          }
        }
      },
    }
  }
};

export const queueVoiceMailMetricsFilterScheme = {
  body: {
    type: 'object',
    properties: {
      startTmpstmp: { type: ['number', 'null'] },
      endTmpstmp: { type: ['number', 'null'] },
      offset: { type: 'number' },
      limit: { type: 'number' },
    },
    required: ['offset', 'limit', 'startTmpstmp', 'endTmpstmp']
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        QueueDetails: {
          type: 'array',
          items: {
            properties: {
              qid: { type: 'number' },
              queueName: { type: 'string' },
            }
          }
        },
        QueueVoiceMail: {
          type: 'array',
          items: {
            properties: {
              TotalCount: { type: 'number' },
              DiffTotalPercent: { type: 'number' },
              CurrentVMRespondedCount: { type: 'number' },
              DiffVMRespondedPercent: { type: 'number' },
              CurrentVMNotRespondedCount: { type: 'number' },
              DiffVMNotRespondedPercent: { type: 'number' },
            }
          }
        },
        QueueVoiceMailTable: {
          type: 'array',
          items: {
            properties: {
              queueName: { type: 'string' },
              VoicemailReceived: { type: 'number' },
              VoicemailResponded: { type: 'number' },
              VoicemailNotResponded: { type: 'number' },
              Total_voicemail_length: { type: 'number' },
              Avg_voicemail_length: { type: 'number' },
            }
          }
        }
      },
    }
  }
};

export const queueServiceLevelMetricsFilterScheme = {
  body: {
    type: 'object',
    properties: {
      startTmpstmp: { type: ['number', 'null'] },
      endTmpstmp: { type: ['number', 'null'] },
      offset: { type: 'number' },
      limit: { type: 'number' },
    },
    required: ['offset', 'limit', 'startTmpstmp', 'endTmpstmp']
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        QueueDetails: {
          type: 'array',
          items: {
            properties: {
              qid: { type: 'number' },
              queueName: { type: 'string' },
            }
          }
        },
        QueueVoiceServiceLevel: {
          type: 'array',
          items: {
            properties: {
              CurrentAnsweredSLACount: { type: 'number' },
              DiffAnsweredSLAPercent: { type: 'number' },
              CurrentAbandonedSLACount: { type: 'number' },
              DiffAbandonedSLAPercent: { type: 'number' },
              CurrentSLAbreachedCount: { type: 'number' },
              DiffSLAbreachedPercent: { type: 'number' },
            }
          }
        },
        QueueVoiceServiceLevelTable: {
          type: 'array',
          items: {
            properties: {
              queueName: { type: 'string' },
              CallType: { type: 'string' },
              SL1: { type: 'number' },
              SL2: { type: 'number' },
              SL3: { type: 'number' },
              SL4: { type: 'number' },
              SL5: { type: 'number' },
              SL6: { type: 'number' },
              TotalSLA: { type: 'number' },
              MissedCalls: { type: 'number' },
              SLAAchieved: { type: 'number' },
              SLAbreached: { type: 'number' },
            }
          }
        }
      },
    }
  }
};

export const queueOutboundCallBestTimeMetricsFilterScheme = {
  body: {
    type: 'object',
    properties: {
      startTmpstmp: { type: ['number', 'null'] },
      endTmpstmp: { type: ['number', 'null'] },
      offset: { type: 'number' },
      limit: { type: 'number' },
    },
    required: ['offset', 'limit', 'startTmpstmp', 'endTmpstmp']
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        QueueDetails: {
          type: 'array',
          items: {
            properties: {
              qid: { type: 'number' },
              queueName: { type: 'string' },
            }
          }
        },
        QueueOutboundCallBestTime: {
          type: 'array',
          items: {
            properties: {
              TotalOutboundCall: { type: 'number' },
              DiffTotalPercent: { type: 'number' },
              CurrentAnsweredeCount: { type: 'number' },
              DiffAnsweredPercent: { type: 'number' },
              CurrentSucessRate: { type: 'number' },
              DiffSucessRatePercent: { type: 'number' },
              CurrRightPartyCount: { type: 'number' },
              DiffRightPartyCount: { type: 'number' },
            }
          }
        },
        QueueOutboundCallBestTimeTable: {
          type: 'array',
          items: {
            properties: {
              Time: { type: 'string' },
              queueName: { type: 'string' },
              outboundcalls: { type: 'number' },
              answeredCalls: { type: 'number' },
              unAnsweredCalls: { type: 'number' },
              SucessRate: { type: 'number' },
            }
          }
        }
      },
    }
  }
};
