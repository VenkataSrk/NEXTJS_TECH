
export const userinsertSchema = {
  body: {
    type: 'object',
    properties: {
      currentStatus: { type: 'string' },
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
export const getUserDetailsSchema = {
  body: {
    type: 'object',
    properties: {
      emailId: { type: 'string' },
      ext: { type: 'string' },

    },
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        result: {
          name: { type: 'string' },
          emailId: { type: 'string' },
          companyId: { type: 'number' },
          domainId: { type: 'number' },
          ext: { type: 'string' },
          createDate: { type: 'string' },
          roleId: { type: 'string' },
          lastModifiedDate: { type: 'string' },
          callType: { type: 'string' },
          channelSupport: { type: 'string' },
          queue: { type: 'string' },
          skillSet: { type: 'string' },
          state: { type: 'string' },
          stateTimer: { type: 'string' },
        }
      },
    },
  },
};

export const activityUpdateSchema = {
  body: {
    type: 'object',
    properties: {
      calledDate: { type: 'string' },
      fromName: { type: 'string' },
      fromExt: { type: 'string' },
      queueID: { type: 'string' },
      queueName: { type: 'string' },
      callType: { type: 'string' },
      talkTime: { type: 'string' },
      wrapTime: { type: 'string' },
      outcome: { type: 'string' },
      recordingUrl: { type: 'string' },
      recordingTranscript: { type: 'string' },
      toExt: { type: 'string' },
      toName: { type: 'string' },
      routingprofileName: { type: 'string' },
      routingProfileId: { type: 'string' },
      callState: { type: 'number' },
      channelSupportId: { type: 'string' },
      channelSupportName: { type: 'string' },
      stateTimer: { type: 'string' },
      disposition: { type: 'string' },
      channelId: { type: 'string' },
      ChannelName: { type: 'string' },
      duration: { type: 'string' },
      queryOutcome: { type: 'string' },
      customerFeedBack: { type: 'string' },
      botFeedback: { type: 'string' },
      companyId: { type: 'number' },
      domainId: { type: 'number' },
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
export const getAllActivitySchema = {
  body: {
    type: 'object',
    properties: {
      emailId: { type: 'string' },
      ext: { type: 'string' },

    },
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        result: {
          calledDate: { type: 'string' },
          customer: { type: 'string' },
          queueID: { type: 'string' },
          queueName: { type: 'string' },
          callType: { type: 'string' },
          talkTime: { type: 'string' },
          wrapTime: { type: 'string' },
          outcome: { type: 'string' },
          recordingUrl: { type: 'string' },
          recordingTranscript: { type: 'string' },
          agent: { type: 'string' },
          routingprofileName: { type: 'string' },
          routingProfileId: { type: 'string' },
          callState: { type: 'number' },
          channelSupportId: { type: 'string' },
          channelSupportName: { type: 'string' },
          stateTimer: { type: 'string' },
          disposition: { type: 'string' },
          channelId: { type: 'string' },
          ChannelName: { type: 'string' },
          queryOutcome: { type: 'string' },
          customerFeedBack: { type: 'string' },
          botFeedback: { type: 'number' },
          companyId: { type: 'number' },
          domainId: { type: 'number' },
          assigned: { type: 'string' },
          assignedTo: { type: 'string' },
        }
      },
    },
  },
};

export const activityGetSchema = {

  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        result: {
          type: 'array',
          items: {
            type: 'object',
            properties: {
              channelId: { type: 'string' },
              date: { type: 'string' },
              from: { type: 'string' },
              to: { type: 'string' },
              callType: { type: 'string' },
              time: { type: 'number' },
              callDuration: { type: 'number' },
              wrapTime: { type: 'number' },
              queryOutcome: { type: 'string' },
              customerFeedback: { type: 'string' },
              BotFeedback: { type: 'string' },
            }
          }
        }
      },
    },
  },
};
export const getCallActivitySchema = {
  body: {
    type: 'object',
    properties: {
      companyId: { type: 'string' },
      channelId: { type: 'string' },

    },
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        result: {
          calledDate: { type: 'string' },
          customer: { type: 'string' },
          queueID: { type: 'string' },
          queueName: { type: 'string' },
          callType: { type: 'string' },
          talkTime: { type: 'string' },
          wrapTime: { type: 'string' },
          outcome: { type: 'string' },
          recordingUrl: { type: 'string' },
          recordingTranscript: { type: 'string' },
          agent: { type: 'string' },
          routingprofileName: { type: 'string' },
          routingProfileId: { type: 'string' },
          callState: { type: 'number' },
          channelSupportId: { type: 'string' },
          channelSupportName: { type: 'string' },
          stateTimer: { type: 'string' },
          disposition: { type: 'string' },
          channelId: { type: 'string' },
          ChannelName: { type: 'string' },
          queryOutcome: { type: 'string' },
          customerFeedBack: { type: 'string' },
          botFeedback: { type: 'string' },
          companyId: { type: 'number' },
          domainId: { type: 'number' },
        }
      },
    },
  },
};
export const getCallBackListSchema = {
  body: {
    type: 'object',
    properties: {
      companyId: { type: 'string' },
      channelId: { type: 'string' },

    },
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        result: {
          calledDate: { type: 'string' },
          customer: { type: 'string' },
          queueID: { type: 'string' },
          queueName: { type: 'string' },
          callType: { type: 'string' },
          talkTime: { type: 'string' },
          wrapTime: { type: 'string' },
          outcome: { type: 'string' },
          recordingUrl: { type: 'string' },
          recordingTranscript: { type: 'string' },
          agent: { type: 'string' },
          routingprofileName: { type: 'string' },
          routingProfileId: { type: 'string' },
          callState: { type: 'number' },
          channelSupportId: { type: 'string' },
          channelSupportName: { type: 'string' },
          stateTimer: { type: 'string' },
          disposition: { type: 'string' },
          channelId: { type: 'string' },
          ChannelName: { type: 'string' },
          queryOutcome: { type: 'string' },
          customerFeedBack: { type: 'string' },
          botFeedback: { type: 'string' },
          companyId: { type: 'number' },
          domainId: { type: 'number' },
          assigned: { type: 'number' },
          assignedTo: { type: 'number' },
        }
      },
    },
  },
};
export const getCallBackExtListSchema = {
  body: {
    type: 'object',
    properties: {
      agent: { type: 'string' },
    },
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        result: {
          calledDate: { type: 'string' },
          customer: { type: 'string' },
          queueID: { type: 'string' },
          queueName: { type: 'string' },
          callType: { type: 'string' },
          talkTime: { type: 'string' },
          wrapTime: { type: 'string' },
          outcome: { type: 'string' },
          recordingUrl: { type: 'string' },
          recordingTranscript: { type: 'string' },
          agent: { type: 'string' },
          routingprofileName: { type: 'string' },
          routingProfileId: { type: 'string' },
          callState: { type: 'number' },
          channelSupportId: { type: 'string' },
          channelSupportName: { type: 'string' },
          stateTimer: { type: 'string' },
          disposition: { type: 'string' },
          channelId: { type: 'string' },
          ChannelName: { type: 'string' },
          queryOutcome: { type: 'string' },
          customerFeedBack: { type: 'string' },
          botFeedback: { type: 'string' },
          companyId: { type: 'number' },
          domainId: { type: 'number' },
          assigned: { type: 'number' },
          assignedTo: { type: 'number' },
        }
      },
    },
  },
};
export const getAgentActivityListSchema = {
  body: {
    type: 'object',
    properties: {
      agent: { type: 'string' },
    },
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        result: {
          calledDate: { type: 'string' },
          customer: { type: 'string' },
          queueID: { type: 'string' },
          queueName: { type: 'string' },
          callType: { type: 'string' },
          talkTime: { type: 'string' },
          wrapTime: { type: 'string' },
          outcome: { type: 'string' },
          recordingUrl: { type: 'string' },
          recordingTranscript: { type: 'string' },
          agent: { type: 'string' },
          routingprofileName: { type: 'string' },
          routingProfileId: { type: 'string' },
          callState: { type: 'number' },
          channelSupportId: { type: 'string' },
          channelSupportName: { type: 'string' },
          stateTimer: { type: 'string' },
          disposition: { type: 'string' },
          channelId: { type: 'string' },
          ChannelName: { type: 'string' },
          queryOutcome: { type: 'string' },
          customerFeedBack: { type: 'string' },
          botFeedback: { type: 'string' },
          companyId: { type: 'number' },
          domainId: { type: 'number' },
          assigned: { type: 'number' },
          assignedTo: { type: 'number' },
        }
      },
    },
  },
};
