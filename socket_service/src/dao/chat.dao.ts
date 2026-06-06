
import axios from 'axios';
import { mysqlPoolConnection } from '../plugins/db';

/**
 *
 * @param data
 * @returns
 */
export const updateMessage = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      const updateq = `UPDATE conversationHistory SET message = '${data}' WHERE id = '${data.id}'`;
      mysqlPoolConnection.query(updateq, async (err, result) => {
        if (err) {
          reject(err);
        } else {
          resolve(result);
        }
      });
    } catch (error) {
      reject(error);
    }
  });
};

// export const createCallSession = async (data: any, callflowid: any) => {
//   try {
//     const initialData: any = {
//       ipAddress: data?.ipAddress ? data?.ipAddress : '',
//       callFlowSourceId: callflowid || '',
//       sessionId: data.session_id,
//       browser: data?.browser ? data?.browser : '',
//       source: data?.source ? data?.source : '',
//       sessionStartTime: new Date().getTime() / 1000,
//       channelType: 'Call',
//       sessionEndTime: data?.sessionEndTime ? data?.sessionEndTime : null,
//       botStartTime: data?.botStartTime ? data?.botStartTime : null,
//       botEndTime: data?.botEndTime ? data?.botEndTime : null,
//       agentConnectTime: data?.agentConnectTime ? data.agentConnectTime : null,
//       agentDisconnectTime: data?.agentDisconnectTime ? data?.agentDisconnectTime : null,
//       lastMessage: data?.lastMessage ? data?.lastMessage : '',
//       botDetails: data?.botDetails ? data?.botDetails : {},
//       category: data?.category ? data?.category : '',
//       isBot: true,
//       agentComments: data?.agentComments ? data.agentComments : '',
//       agentDetails: data?.agentDetails ? data.agentDetails : '',
//       customerDetails: data?.cli ? data.cli : '',
//       OS: data?.OS ? data?.OS : '',
//       onlineStatus: true,
//       skillsIdentify: data?.skillsIdentify ? data?.skillsIdentify : '',
//       location: data?.location ? data?.location : '',
//       deviceType: data?.deviceType ? data?.deviceType : '',
//     };

//     return new Promise(async (resolve, reject) => {
//       /*   sessionHistoryModel.find({ chatSessionId: initialData.chatSessionId }).then((res) => {
//           ;

//           if (res && res.length > 0) {
//             sessionHistoryModel.updateOne({ chatSessionId: initialData.chatSessionId }, { $set: initialData }).then((result) => {

//               return true;
//             }).catch((err) => {
//               return err;
//             });
//           } else {
//             ;
//             sessionHistoryModel.create(initialData)
//             return true;
//           }
//         }).catch((err) => {
//           return err;
//         }); */
//       sessionHistoryModel
//         .create(initialData)
//         .then((response: any) => {
//           resolve(response);
//         })
//         .catch((errors: any) => {
//           reject(errors);
//         });
//     });
//   } catch (err: any) {
//     console.log(err);
//   }
// };

/* export const createSession = async (data: any, Id: any) => {
  try {
    const initialData: any = {
      ipAddress: data?.ipAddress ? data?.ipAddress : '',
      callFlowSourceId: data?.webChatId ? data.webChatId : '',
      sessionId: Id,
      browser: data?.browser ? data?.browser : '',
      source: data?.source ? data?.source : '',
      sessionStartTime: new Date().getTime() / 1000,
      channelType: 'Chat',
      sessionEndTime: data?.sessionEndTime ? data?.sessionEndTime : null,
      botStartTime: data?.botStartTime ? data?.botStartTime : null,
      botEndTime: data?.botEndTime ? data?.botEndTime : null,
      agentConnectTime: data?.agentConnectTime ? data.agentConnectTime : null,
      agentDisconnectTime: data?.agentDisconnectTime
        ? data?.agentDisconnectTime
        : null,
      lastMessage: data?.lastMessage ? data?.lastMessage : '',
      botDetails: data?.botDetails ? data?.botDetails : {},
      category: data?.category ? data?.category : '',
      isBot: true,
      agentComments: data?.agentComments ? data.agentComments : '',
      agentDetails: data?.agentDetails ? data.agentDetails : '',
      customerDetails: '',
      OS: data?.OS ? data?.OS : '',
      onlineStatus: true,
      skillsIdentify: data?.skillsIdentify ? data?.skillsIdentify : '',
      location: data?.location ? data?.location : '',
      deviceType: data?.deviceType ? data?.deviceType : '',
    };

    return new Promise(async (resolve, reject) => {
      sessionHistoryModel
        .find({ sessionId: initialData.sessionId })
        .then((res) => {
          if (res && res.length > 0) {
            sessionHistoryModel
              .updateOne(
                { sessionId: initialData.sessionId },
                { $set: initialData }
              )
              .then((result) => {
                return true;
              })
              .catch((err) => {
                return err;
              });
          } else {
            sessionHistoryModel.create(initialData);
            return true;
          }
        })
        .catch((err) => {
          return err;
        });
    });
  } catch (err: any) {
    console.log(err);
  }
}; */

/**
 *  chat history initial data
 */
export const createSession = (data: any, id: any) => {
  console.log('<<<<<<<<<<<<<<<<<<<<<data>>>>>>>>>>>>>>>>>>>>>>>>>>>>', data);
  // console.log('<<<<<<<<<<<<<<<<<<<<<<message>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>', JSON.stringify(data?.message));
  return new Promise(async (resolve, reject) => {
    try {
      const botDetails = JSON.stringify(data?.botDetails);
      const customerData = JSON.stringify(data?.customerData);
      const message = JSON.stringify(data?.message);
      const customerCallBackData = JSON.stringify(data?.callBackRequest);
      mysqlPoolConnection.query(
        'call ccaas_session_history_details(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)',
        [
          id,
          data?.sessionStartTime ?? null,
          data?.sessionEndTime ?? null,
          data?.botStartTime ?? null,
          data?.botEndTime ?? null,
          data?.agentConnectTime ?? null,
          data?.agentDisconnectTime ?? null,
          data?.ipAddress ?? null,
          data?.source ?? null,
          data?.lastMessage ?? null,
          botDetails ?? null,
          data?.category ?? null,
          true,
          data?.agentComments ?? null,
          data?.agentDetails ?? null,
          data?.cli ?? null,
          data?.browser ?? null,
          data?.OS ?? null,
          true,
          data?.skillsIdentify ?? null,
          data?.callFlowSourceId ?? null,
          data?.channelType ?? 'Chat',
          data?.location ?? null,
          data?.deviceType ?? null,
          customerData ?? null,
          data.domain_id ?? null,
          data?.companyId ?? null,
          data?.disposition ?? null,
          data?.status ?? null,
          data?.priority ?? null,
          data?.summary ?? null,
          data?.follow_up_action ?? null,
          data?.recordingUrl ?? null,
          data?.callType ?? null,
          data?.queue ?? null,
          data?.wrapTime ?? null,
          data?.botDuration ?? null,
          data?.agentDuration ?? null,
          data?.agentRecordingUrl ?? null,
          data?.disconnectedBy ?? null,
          data?.qid ?? null,
          data?.assigned ?? null,
          data?.assignedTo ?? null,
          data?.assignedBy ?? null,
          data?.waitDuration ?? null,
          data?.agentHoldDuration ?? null,
          data?.dialDuration ?? null,
          data?.calledNumber ?? null,
          data?.queueDuration ?? null,
          data?.queueName ?? null,
          data?.callDuration ?? null,
          message ?? null,
          data?.customerFile ?? null,
          data?.agentName ?? null,
          data?.skillName ?? null,
          data?.dispositionName ?? null,
          data?.afterCallWorkTime ?? null,
          data?.callRecorded === 1 ? 1 : 0, // Recording type: If call is recorded or not
          data?.callDuration ?? null, // Recording duration: Same as call duration
          data?.transferred ?? null,
          data?.activeChatTime ?? null,
          data?.responseTime ?? null,
          data?.assignedOn ?? null,
          data?.chatSession ?? null,
          data?.transferredTo ?? null,
          data?.markAsRead ?? null,
          data?.agentStatus ?? null,
          data?.calledPersonDetails ?? null,
          data?.subject ?? null,
          data?.primarySkill ?? null,
          data?.primaryAgent ?? null,
          data?.transferredSkill ?? null,
          data?.transferredAgent ?? null,
          data?.primaryQueue ?? null,
          data?.transferredQueue ?? null,
          data?.transferredTime ?? null,
          data?.transferredType ?? null,
          data?.voicemailUrl ?? null,
          data?.vmsTranscript ?? null,
          customerCallBackData ?? null,
          data?.supervisorName ?? null,
          data?.ticketId ?? null,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            console.log(err);
          }
          resolve(result);
        }
      );
    } catch (err) {
      reject(err);
    }
  });
};
// export const createSession = (data: any, id: any) => {
//   console.log('1 createSession data message: ', data);
//  // console.log('createSession message: ', JSON.stringify(data?.message));
//   return new Promise(async (resolve, reject) => {
//     try {
//       const message = JSON.stringify(data?.message);
//       mysqlPoolConnection.query(
//         'call ccaas_create_call_session_history_details(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)',
//         [
//           data?.session_id,
//           data?.sessionStartTime,
//           data?.sessionEndTime ?? null,
//           data?.cli ?? null,
//           data?.callFlowSourceId ?? null,
//           data?.companyId ?? null,
//           data.domain_id ?? null,
//           data?.dispositionId ?? null,
//           data?.agentStatus ?? null,
//           data?.callType === 'inbound' ? 1 : 0 ?? null,
//           data?.dialDuration ?? null,
//           data?.disconnectedBy === '' ? null : null ?? null,
//           data?.queueDuration ?? null,
//           data?.assignedTo ?? null,
//           data?.assignedBy ?? null,
//           data?.assignedOn ?? null,
//           data?.calledNumber ? Number(data?.calledNumber) : null,
//           data?.waitDuration ?? null,
//           data?.afterCallWorkTime ?? null,
//           data?.recordingTypeId ?? null,
//           data?.istransferred ?? null,
//           data?.transferredTypeId ?? null,
//           data?.markAsRead ?? null,
//           data?.customerId ?? null,
//           data?.voicemailUrl ?? null,
//           data?.ticketId ?? null,
//           data?.agentConnectTime ?? null,
//           data?.agentDisconnectTime ?? null,
//           data?.agentDuration ?? null,
//           data?.agentHoldDuration ?? null,
//           data?.agentId ?? null,
//           data?.agentSkillId ?? null,
//           data?.isBot ?? null,
//           data?.botId ?? null,
//           data?.recordingUrl ?? null,
//           data?.transferredTime ?? null,
//           data?.recordingDuration ?? null,
//           data?.qid ? Number(data?.qid) : 0,
//           message ?? null,
//           data?.callHistoryId ?? null,
//           data?.callDuration ?? null,
//         ],
//         (err: any, result: any) => {
//           if (err) {
//             reject(err);
//             console.log(err);
//           }
//           resolve(result);
//         }
//       );
//     } catch (err) {
//       reject(err);
//     }
//   });
// };

/**
 *
 * @param data
 * @returns get Message WebChat
 */
export const getMessageWebChat = (id: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_GetChatMessageById(?)',
        [
          id ?? 0,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
          }
          resolve(result);
        }
      );
    } catch (error) {
      reject(error);
    }
  });
};

/**
 * @param data
 * @returns
 */
export const storeMessageHistory = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      // const message = JSON.stringify(data.message);
      mysqlPoolConnection.query(
        'call ccass_create_conv_history_details(?,?,?,?,?,?,?,?,?,?,?,?,?)',
        [
          data.sessionId,
          data.from,
          data.to,
          data.messageType,
          data.message,
          data.id,
          data.timeStamp,
          data.agentState,
          data.customerState,
          data.botDetails,
          data.domainId,
          data.companyId,
          data.isBot,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
          }
          resolve(result);
        }
      );
      const mysqlqs = `UPDATE sessionHistory SET lastMessage= '${data?.message}' WHERE sessionId= '${data?.sessionId}'`;
      mysqlPoolConnection.query(mysqlqs, (err, results) => {
        if (err) {
          reject(err);
        } else {
          resolve(results);
        }
      });
    } catch (error) {
      console.log(error);
    }
  });
};

export const getAllUserList = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_getAllUserList_info(?,?)',
        [data?.domainId, data?.ext ?? 0],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            console.log('getAllUserList reject err: ', err);
          } else {
            resolve(result.length > 0 ? result[0] : []);
          }
        }
      );
    } catch (error) {
      console.log('getAllUserList catch error: ', error);
      reject(error);
    }
  });
};

/* export const storeMessageHistory = async (data: any) => {
  try {
    return new Promise(async (resolve, reject) => {
      conversationHistoryModel
        .create(data)
        .then((result) => {
          resolve(result);
          sessionHistoryModel
            .updateOne(
              { sessionId: data.sessionId },
              { $set: { lastMessage: data.message } }
            )
            .then((rsp) => {
              return true;
            });
        })
        .catch((err) => {
          reject(err);
        });
    });
  } catch (err: any) {
    console.log(err);
  }
}; */

/**
 *
 * @param id
 * @param number
 * @returns
 */
export const storeCustomerDetails = async (id: any, number: any) => {
  return new Promise((resolve, reject) => {
    try {
      const mysqlq = `SELECT * FROM customer_contact WHERE phoneNumber = '${number}'`;
      mysqlPoolConnection.query(mysqlq, (err, result) => {
        if (err) {
          reject(err);
        } else {
          if (result.length !== 0) {
            const response = JSON.parse(JSON.stringify(result) ?? 'null');
            const updateQuery = `UPDATE sessionHistory SET customerData='${JSON.stringify(response[0])}' WHERE sessionId='${id}'`;
            mysqlPoolConnection.query(updateQuery, (error, res) => {
              if (error) {
                reject(error);
              } else {
                resolve(res);
              }
            });
          } else {
            const updateQuery = `UPDATE sessionHistory SET customerData='{}' WHERE sessionId='${id}'`;
            mysqlPoolConnection.query(updateQuery, (error, res) => {
              if (error) {
                reject(error);
              } else {
                resolve(res);
              }
            });
          }
        }
      });
    } catch (error) {
      reject(error);
    }
  });
};

/* export const storeCustomerDetails = async (id: any, number: any) => {
  try {
    return new Promise(async (resolve, reject) => {
      await axios
        .post(
          'https://liveurapi.worktual.co.uk/vectone-myaccount/v1/user/Websitecentralgetcustomerinfo',
          { mundio_product: 'vmuk', searchby: 'mobileno', searchvalue: number },
          { headers: { Authorization: 'eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1dWlkIjoiN2UwNTJmYTAtMzc4Ni00MDZjLWJiNjctZDYxNzFlOWJkMTlhIiwidXNlcm5hbWUiOiJwZXJzaGliYS52QHZlY3RvbmUuY29tIiwiZGV2aWNlSWQiOiJhMTJhZWVjMi1iNGJiLTRjZjEtYmI1OC1kZmQ1YzQzOWZlZTkiLCJzaXBMb2dpbklkIjoiNTQ0OCIsInJvbGVJZCI6NCwiZG9tYWluSWQiOjMzMjgsImV4dCI6NTg0LCJjb21wYW55SWQiOjMzNjIsImVuZXRlcHJpc2VpZCI6ODAzMiwiaG9zdEFkZHJlc3MiOiI4MDMyLnVyY2hhdC51bmlmaWVkcmluZy5jby51ayIsIm9yZGVySWQiOjExMDE4LCJkaXJVc2VySWQiOjMyNzA2LCJwcm9maWxlTmFtZSI6IlBlcnNoaWJhIFZlbHVzYW15IiwicHJvZmlsZUltZyI6Imh0dHBzOi8vdXJzdG9yYWdlLnVuaWZpZWRyaW5nLmNvLnVrL2ZpbGVzL3VzZXIvMzM2Mi81NDQ4XzU4NC8xNjUzNTQ1MTQ1MzQwLzEwODBwLWdpcmwtQmVhdXRpZnVsLVdoYXRzYXBwLURwLVByb2ZpbGUtSW1hZ2VzLXBob3RvLWhkLmpwZWciLCJzb3VyY2UiOiJXZWIiLCJpcGFkZHJlc3MiOiIxMzYuMTQ0LjU2LjI1NTo0NDMiLCJpYXQiOjE2NTU0NDI4OTZ9.yhEcej3x7miIQebCQRrAzAznckyLWhHkYslIPKa-BGk' } }
        )
        .then((result: any) => {
          if (result.data.result) {
            sessionHistoryModel
              .updateOne(
                { sessionId: id },
                { $set: { customerData: result.data.result[0] } }
              )
              .then((res: any) => {
                resolve(res);
              })
              .catch((err: any) => {
                reject(err);
              });
          } else {
            sessionHistoryModel
              .updateOne({ sessionId: id }, { $set: { customerData: {} } })
              .then((res: any) => {
                resolve(res);
              })
              .catch((err: any) => {
                reject(err);
              });
          }
        })
        .catch((err: any) => {
          reject(err);
        });
    });
  } catch (err: any) {
    console.log(err);
  }
};

/* export const storeCallHistory = async (data: any) => {
  // ;
  try {
    return new Promise(async (resolve, reject) => {
      conversationHistoryModel
        .create(data)
        .then((result) => {
          resolve(result);
        })
        .catch((err) => {
          reject(err);
        });
    });
  } catch (err: any) {
    console.log(err);
  }
}; */

/**
 *
 * @param data
 * @returns get Chat History
 */
export const getChatHistory = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      const mysqlq = `SELECT * FROM conversationHistory where sessionId = '${data}'`;
      mysqlPoolConnection.query(mysqlq, (err, result) => {
        if (err) {
          reject(err);
        } else {
          resolve(result);
        }
      });
    } catch (error) {
      reject(error);
    }
  });
};

/* export const getChatHistory = async (id: any) => {
  try {
    return new Promise(async (resolve, reject) => {
      conversationHistoryModel
        .find({ sessionId: id })
        .then((result) => {
          resolve(result);
        })
        .catch((err) => {
          reject(err);
        });
    });
  } catch (err: any) {
    console.log(err);
  }
}; */

// export const chatUpdateDao = async (data: any) => {
//   try {
//     // ;

//     return new Promise(async (resolve, reject) => {
//       sessionHistoryModel
//         .updateOne({ sessionId: data.sessionId }, { $set: data })
//         .then((res: any) => {
//           resolve(res);
//         })
//         .catch((err: any) => {
//           reject(err);
//         });
//     });
//   } catch (err: any) {
//     console.log(err);
//   }
// };

/**
 *  call history initial data
 */
export const chatUpdateDao = (data: any) => {
  console.log('chatUpdateDao>>>>>>>>', data);
  console.log('<<<<<<<<<<<<<<<<<<<<<<chatUpdateDao>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>', JSON.stringify(data?.message));
  return new Promise(async (resolve, reject) => {
    const botDetails = JSON.stringify(data?.botDetails);
    const customerData = JSON.stringify(data?.customerData);
    const message = JSON.stringify(data?.message);
    const domainId = parseInt(data?.domain_id, 10);
    try {
      mysqlPoolConnection.query(
        'call ccaas_session_history_details(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,??,?,?,?,?,?,?,?,?,?,?,?,?,?,?)',
        [
          data?.session_id,
          data?.sessionStartTime ?? null,
          data?.sessionEndTime ?? null,
          data?.botStartTime ?? null,
          data?.botEndTime ?? null,
          data?.agentConnectTime ?? null,
          data?.agentDisconnectTime ?? null,
          data?.ipAddress ?? null,
          data?.source ?? null,
          data?.lastMessage ?? null,
          botDetails ?? null,
          data?.category ?? null,
          true,
          data?.agentComments ?? null,
          data?.agentDetails ?? null,
          data?.cli ?? null,
          data?.browser ?? null,
          data?.OS ?? null,
          true,
          data?.skillsIdentify ?? null,
          data?.callFlowSourceId ?? null,
          'Call',
          data?.location ?? null,
          data?.deviceType ?? null,
          customerData ?? null,
          domainId ?? null,
          data?.companyId ?? null,
          data?.disposition ?? null,
          data?.status ?? null,
          data?.priority ?? null,
          data?.summary ?? null,
          data?.follow_up_action ?? null,
          data?.recordingUrl ?? null,
          data?.callType ?? null,
          data?.queue ?? null,
          data?.wrapTime ?? null,
          data?.botDuration ?? null,
          data?.agentDuration ?? null,
          data?.agentRecordingUrl ?? null,
          data?.disconnectedBy ?? null,
          data?.qid ?? null,
          data?.assigned ?? null,
          data?.assignedTo ?? null,
          data?.assignedBy ?? null,
          data?.waitDuration ?? null,
          data?.agentHoldDuration ?? null,
          data?.dialDuration ?? null,
          data?.calledNumber ?? null,
          data?.queueDuration ?? null,
          data?.queueName ?? null,
          data?.callDuration ?? null,
          message ?? null,
          data?.customerFile ?? null,
          data?.agentName ?? null,
          data?.skillName ?? null,
          data?.dispositionName ?? null,
          data?.afterCallWorkTime ?? null,
          data?.callRecorded === 1 ? 1 : 0, // Recording type: If call is recorded or not
          data?.callDuration ?? null, // Recording duration: Same as call duration
          data?.transferred ?? null,
          data?.activeChatTime ?? null,
          data?.responseTime ?? null,
          data?.assignedOn ?? null,
          data?.chatSession ?? null,
          data?.transferredTo ?? null,
          data?.markAsRead ?? null,
          data?.agentStatus ?? null,
          data?.calledPersonDetails ?? null,
          data?.subject ?? null,
          data?.primarySkill ?? null,
          data?.primaryAgent ?? null,
          data?.transferredSkill ?? null,
          data?.transferredAgent ?? null,
          data?.primaryQueue ?? null,
          data?.transferredQueue ?? null,
          data?.transferredTime ?? null,
          data?.transferredType ?? null,
          data?.voicemailUrl ?? null,
          data?.vmsTranscript ?? null,
          data?.callBackDetails ?? null,
          data?.supervisorName ?? null,
          data?.ticketId ?? null,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            console.log(err);
          }
          resolve(result);
        }
      );
    } catch (err) {
      reject(err);
    }
  });
};

/* export const getChatSessionDao = async (ext: number) => {
  try {
    return new Promise(async (resolve, reject) => {
      sessionHistoryModel
        .find({ agentDetails: ext, channelType: 'Chat' })
        .then((res: any) => {
          resolve(res);
        })
        .catch((err: any) => {
          reject(err);
        });
    });
  } catch (err: any) {
    console.log(err);
  }
}; */

/**
 * @description get Chat Session
 * @param data
 * @returns
 */
export const getChatSessionDao = (ext: any) => {
  return new Promise((resolve, reject) => {
    try {
      const mysqlq = `SELECT * FROM sessionHistory where agentDetails = ${ext} AND channelType: 'Chat'`;
      mysqlPoolConnection.query(mysqlq, (err, result) => {
        if (err) {
          reject(err);
        } else {
          resolve(result);
        }
      });
    } catch (error) {
      reject(error);
    }
  });
};
/**
 *
 * @param ext
 * @returns
 */
/* export const getCallSessionDao = async (ext: number) => {
  try {
    return new Promise(async (resolve, reject) => {
      sessionHistoryModel
        .find({ agentDetails: ext, channelType: 'Call' })
        .then((res: any) => {
          resolve(res);
        })
        .catch((err: any) => {
          reject(err);
        });
    });
  } catch (err: any) {
    console.log(err);
  }
}; */

/**
 * @description get Call Session
 * @param data
 * @returns
 */
export const getCallSessionDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      const mysqlq = `SELECT * FROM sessionHistory where domainId = ${data.domainId} AND isBot = 0 AND agentDetails is not null AND dispositionName is null`;
      mysqlPoolConnection.query(mysqlq, (err: any, result: any) => {
        if (err) {
          reject(err);
        } else {
          resolve(result);
        }
      });
    } catch (error) {
      reject(error);
    }
  });
};
/* export const getChatSessionIdDao = async (id: string) => {
  try {
    return new Promise(async (resolve, reject) => {
      sessionHistoryModel
        .findOne({ sessionId: id })
        .then((res: any) => {
          resolve(res);
        })
        .catch((err: any) => {
          reject(err);
        });
    });
  } catch (err: any) {
    console.log(err);
  }
}; */

/**
 * @description get the chat session
 * @param data
 * @returns
 */
export const getChatSessionIdDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      console.log(data, 'datadatadatadatadatadatadatadatadata');
      const mysqlq = `SELECT * FROM tb_ChatHistory where ChatHistoryId=${data}`;
      mysqlPoolConnection.query(mysqlq, (err, result) => {
        if (err) {
          reject(err);
        } else {
          console.log('result-------------', result);
          resolve(result);
        }
      });
    } catch (error) {
      reject(error);
    }
  });
};

/**
 * @description get the Call session
 * @param data
 * @returns
 */
export const getCallSessionIdDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      const mysqlq = `SELECT * FROM tb_CallHistory where sessionId = '${data}'`;
      mysqlPoolConnection.query(mysqlq, (err, result) => {
        if (err) {
          reject(err);
        } else {
          resolve(result);
        }
      });
    } catch (error) {
      reject(error);
    }
  });
};

/**
 * @description get chat number from
 * @param data
 * @returns
 */
// check with db team customerNumber
export const getChatByNumberDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      if (data) {
        // const mysqlq = `SELECT * FROM sessionHistory where customerDetails = '${data}'`;
        const mysqlq = `SELECT * FROM tb_CallHistory where customerNumber = '${data}'`;
        mysqlPoolConnection.query(mysqlq, (err, result) => {
          if (err) {
            reject(err);
          } else {
            resolve(result);
          }
        });
      } else {
        resolve([]);
      }
    } catch (error) {
      reject(error);
    }
  });
};

/**
 * @description get chat by endTime
 * @param data
 * @returns
 */
export const getChatByEndTimeDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      const mysqlq = `SELECT * FROM sessionHistory where agentDetails = ${data} AND wrapTime = 0`;
      mysqlPoolConnection.query(mysqlq, (err, result) => {
        if (err) {
          reject(err);
        } else {
          resolve(result);
        }
      });
    } catch (error) {
      reject(error);
    }
  });
};
/* export const getChatByNumberDao = async (number: string) => {
  try {
    return new Promise(async (resolve, reject) => {
      sessionHistoryModel
        .find({ customerDetails: number })
        .then((res: any) => {
          resolve(res);
        })
        .catch((err: any) => {
          reject(err);
        });
    });
  } catch (err: any) {
    console.log(err);
  }
}; */

/* export const getChatByEndTimeDao = async (ext: number) => {
  try {
    return new Promise(async (resolve, reject) => {
      sessionHistoryModel
        .find({ agentDetails: ext, wrapTime: null })
        .then((res: any) => {
          resolve(res);
        })
        .catch((err: any) => {
          reject(err);
        });
    });
  } catch (err: any) {
    console.log(err);
  }
}; */

/**
 *
 * @param data
 * @returns get Message WebChat
 */
export const createCoChatChannel = (channelDetails: any) => {
  return new Promise((resolve, reject) => {
    try {
      const mysqlq = `INSERT INTO coChat (subscribedUsers, customerInfo, channelId, uid, channelName) VALUES (${channelDetails?.agentListData}, ${channelDetails?.customerDetails}, ${channelDetails?.channelId}, ${channelDetails?.coid}, ${channelDetails?.channelName}) `;
      mysqlPoolConnection.query(mysqlq, (err, result) => {
        if (err) {
          reject(err);
        } else {
          resolve(result);
        }
      });
    } catch (error) {
      reject(error);
    }
  });
};

/**
 *
 * @param data
 * @param dataq
 * @returns
 */

export const coChatUpdateDao = async (data: any, dataq: any) => {
  const query = `UPDATE coChat SET subscribedUsers = CONCAT(subscribedUsers, ?) WHERE uid = '${dataq.uid}' AND channelId = '${dataq.channelId}' AND  domainId = ${dataq.domainId}`;
  const values = [JSON.stringify(data), dataq.id];
  try {
    return new Promise(async (resolve, reject) => {
      mysqlPoolConnection.query(query, values, (err, res) => {
        if (err) {
          reject(err);
        } else {
          resolve(res);
        }
      });
    });
  } catch (err) {
    console.log(err);
  }
};

/**
 * @description
 * @param data
 * @returns
 */
export const getAllMessageId = (channelId: any, domainId: any) => {
  return new Promise((resolve, reject) => {
    try {
      const mysqlq = `SELECT * FROM coChat WHERE channelId = '${channelId}' AND domainId = ${domainId}`;
      mysqlPoolConnection.query(mysqlq, (err, result) => {
        if (err) {
          reject(err);
        } else {
          resolve(result);
        }
      });
    } catch (error) {
      reject(error);
    }
  });
};

/**
 *
 * @param data
 * @param dataq
 * @returns
 */

export const sendMessageToCoChat = async (data: any, dataq: any) => {
  const query = `UPDATE coChat SET messages = CONCAT(messages, ?) WHERE uid = '${dataq.uid}' AND channelId = '${dataq.channelId}' AND  domainId = ${dataq.domainId}`;
  const values = [JSON.stringify(data), dataq.id];
  try {
    return new Promise(async (resolve, reject) => {
      mysqlPoolConnection.query(query, values, (err, res) => {
        if (err) {
          reject(err);
        } else {
          resolve(res);
        }
      });
    });
  } catch (err) {
    console.log(err);
  }
};

export const availableAgentData = async (data: any) => {
  const query = `SELECT * from user where domainId=${data.domain_id} AND statusName='Ready'`;
  try {
    return new Promise(async (resolve, reject) => {
      mysqlPoolConnection.query(query, (err: any, res: any) => {
        if (err) {
          reject(err);
        } else {
          resolve(res);
        }
      });
    });
  } catch (err) {
    console.log(err);
  }
};

export const storeCochatDetailsByIDDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      const mysqlMessage = `SELECT messages FROM coChat Where domainId = ${data?.domainId ?? null} AND channelId = '${data?.channelId ?? null}'`;
      mysqlPoolConnection.query(mysqlMessage, (err, result) => {
        if (err) {
          reject(err);
        } else {
          let resultMessage: any = [];
          if (result?.length > 0 && result[0]?.messages?.length > 0) {
            resultMessage = JSON.parse(result[0]?.messages ?? 'null');
            resultMessage.push(data);
          } else {
            resultMessage.push(data);
          }
          const parseResultMessage = JSON.stringify(resultMessage);
          const mysqlq = `UPDATE coChat SET messages='${parseResultMessage}' WHERE domainId = ${data?.domainId ?? null} AND channelId = '${data.channelId}'`;
          mysqlPoolConnection.query(mysqlq, (errs, result1) => {
            if (errs) {
              reject(errs);
            } else {
              resolve(result1);
            }
          });
        }
      });
    } catch (error) {
      reject(error);
    }
  });
};

export const saveChatFlowDao = (data: any, nodeData: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_InsertUpdatechatFlowHistory(?,?,?,?,?,?,?,?,?,?,?,?,?)',
        [
          data?.cfid,
          data?.templateName,
          nodeData?.node,
          nodeData?.edge,
          data?.version,
          data?.type,
          data?.status,
          data?.companyId,
          data?.domainId,
          data?.description,
          data?.publishedDataArr,
          data?.action,
          data?.category
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            console.log(err);
          }
          resolve(result);
        }
      );
    } catch (err) {
      reject(err);
    }
  });
};
export const getChatFlowIdDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      const mysqlq = `SELECT * FROM chatFlowHistory where chatFlowId='${data.chatFlowId}' AND domainId=${data.domainId}`;
      mysqlPoolConnection.query(mysqlq, (err: any, result: any) => {
        if (err) {
          reject(err);
        } else {
          resolve(result);
        }
      });
    } catch (error) {
      reject(error);
    }
  });
};
export const getAllChatFlowIdDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      const mysqlq = `SELECT * FROM chatFlowHistory where domainId=${data.domainId} ORDER BY chatFlowId DESC`;
      mysqlPoolConnection.query(mysqlq, (err: any, result: any) => {
        if (err) {
          reject(err);
        } else {
          resolve(result);
        }
      });
    } catch (error) {
      reject(error);
    }
  });
};

export const getAllSessionsDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      const mysqlq = `SELECT * FROM sessionHistory WHERE domainId=${data.domainId} order by shid desc limit ${data.limit} offset ${data.offset}`;
      mysqlPoolConnection.query(mysqlq, (err: any, result: any) => {
        if (err) {
          reject(err);
        } else {
          resolve(result);
        }
      });
    } catch (error) {
      reject(error);
    }
  });
};

/**
 *
 * @param data
 * @returns get Message WebChat
 */
export const getLeadCRMinformation = async (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      const query: any = `SELECT * FROM tb_LeadGeneration WHERE domainId=${data?.domainId}`;
      mysqlPoolConnection.query(query, (err: any, result: any) => {
        if (err) {
          reject(err);
          console.log(err);
        }
        // console.log('LeadGeneration>>>>>>>>>>>>>>>', result);
        if (result && result?.length > 0) {
          const dataRes = result[0]?.connector;
          if (dataRes?.includes('Worktual')) {
            const worktualList = {
              First_Name: data?.first_name,
              Last_Name: data?.last_name,
              Contact_owner: '1',
              Phone_number: data?.mobile_no,
              Email: data?.email,
              p_UserId: 5,
              p_CompanyId: data?.companyId,
              user_contact_id: 0  // if contact exist , give user_contact_id
            };
            // console.log('dataRes>>>>>>>>>>', dataRes);
            axios.post('https://stagemarketingapi.worktual.co.uk/marketing_automation/v1/contact/create_contact', worktualList).then((response) => {
              console.log(response, 'response');
            }).catch((error: any) => { console.log(error); });
          }
          if (dataRes?.includes('Salesforce')) {
            const salesForceList = {
              FirstName: data?.first_name,
              LastName: data?.first_name,
              OtherStreet: data?.address ?? null,
              Phone: data?.mobile_no ?? null,
              Email: data?.email

            };
            mysqlPoolConnection.query(
              'call ccaas_get_crm_connector_token(?)', [data?.domainId],
              (err1: any, result1: any) => {
                if (err1) {
                  reject(err1);
                  console.log(err1);
                }

                const nextNode = result1[0]?.find((nodes: any) => nodes?.connector_name === 'salesforce');
                // console.log(nextNode?.token,"yfuwyediwyrweuwooppop")
                axios.post('https://ccaasapi.worktual.co.uk/connector/v1/salesforce/contacts', salesForceList, {
                  headers: {
                    'Content-Type': 'application/json',
                    Authorization: nextNode?.token
                  }
                }).then(async (response: any) => {
                  if (response?.data.statusCode === 401) {
                    const refersh: any = await getSalesforceToken(data?.domainId, nextNode?.token);

                    axios.post('https://ccaasapi.worktual.co.uk/connector/v1/salesforce/contacts', salesForceList, {
                      headers: {
                        'Content-Type': 'application/json',
                        Authorization: refersh?.data?.token
                      }
                    }).then((response1: any) => {

                      // console.log(response1, 'response');
                    }).catch((error) => { console.log(error); });
                  }
                  // console.log(response,"response")
                }).catch((error) => { console.log(error); });
              }
            );
          }
        }
        // const list = {
        //   First_Name: data?.firstName,
        //   Last_Name: data?.lastName,
        //   Contact_owner: '1',
        //   Phone_number: data?.primaryPhoneNumber,
        //   Email: data?.primaryEmail,
        //   p_UserId: 5,
        //   p_CompanyId: data?.companyId,
        //   user_contact_id: 0  // if contact exist , give user_contact_id
        // };
        // axios.post('https://stagemarketingapi.worktual.co.uk/marketing_automation/v1/contact/create_contact', list).then((response) => {
        //   console.log(response, 'response');
        // }).catch((error: any) => { console.log('create_contact errr>>>>', error); });

        // if (data?.primaryEmail) {
        //   const payload = {
        //     address: data?.address,
        //     city: data?.city,
        //     company: data?.companyId,
        //     country: data?.country,
        //     website: '',
        //     email: data?.primaryEmail,
        //     firstName: data?.firstName,
        //     phone: data?.primaryPhoneNumber,
        //     message: '',
        //     lastName: data?.lastName
        //   };
        //   axios.post('https://mysqlapi.worktual.co.uk/urpricing-mysql/v1/contactUs_hubspot_form', payload).then((response: any) => {
        //     if (response.status === 200) {
        //       console.log('contactUs_hubspot_form>>>>>', response.data);
        //       if (response?.data.statusCode === 401 && response?.data?.errCode === -1 && response?.data?.message?.message.includes('Contact already exists')) {
        //         const hubspotId = response.data?.message?.message?.split(':')?.[1];
        //         const updatePayload = { ...payload, hubspotId: Number(hubspotId) };
        //         axios.post('https://mysqlapi.worktual.co.uk/urpricing-mysql/v1/contactUs_hubspot_form_update', updatePayload).then((updateResponse: any) => {
        //           console.log('contactUs_hubspot_form_update>>>>', updateResponse.data);
        //         }).catch((error: any) => { console.log('contactUs_hubspot_form_update err>>>>', error); });
        //       }
        //     }
        //   }).catch((error: any) => { console.log('contactUs_hubspot_form errr>>>>', error); });
        // }
        resolve(result);
      });

    } catch (error) {
      reject(error);
    }
  });
};

export const getSalesforceToken = (domainId: any, token: any) => {
  return new Promise((resolve, reject) => {
    try {
      axios.get(`https://ccaasapi.worktual.co.uk/connector/v1/salesforce/refresh_token/${domainId}`, {
        headers: {
          'Content-Type': 'application/json',
          Authorization: token
        }
      }).then((res: any) => {
        resolve(res);
        // console.log(res,"response88888888888")
      }).catch((error) => { console.log(error); });
    } catch (error) {
      reject(error);
    }
  });
};

/**
 *
 * @param data
 * @param dataq
 * @returns
 */
export const getUserList = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_getUserListByRoleHandler(?,?)',
        [data?.domainId, '1,2,3,4,5,6'],
        (err: any, result: any) => {
          if (err) {
            reject(err);
          } else {
            if (result && result[0]?.length > 0) {
              resolve(result[0]);
            } else {
              resolve([]);
            }
          }
        }
      );
    } catch (error) {
      reject(error);
    }
  });
};

/**
 *
 * @param data
 * @param dataq
 * @returns
 */
export const agentExtSocketIdMapping = (data: any, id: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL InsertChatConnection(?,?,?)',
        [data?.domainId, data?.ext, id],
        (err: any, result: any) => {
          console.log('result', result);
          console.log('err', err);
          if (err) {
            reject(err);
          } else {
            if (result && result[0]?.length > 0) {
              resolve(result[0]);
            } else {
              resolve([]);
            }
          }
        }
      );
    } catch (error) {
      reject(error);
    }
  });
};

/**
 *
 * @param data
 * @param dataq
 * @returns
 */
export const getAgentDetails = (id: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL GetChatConnectionBySocketId(?)',
        [id],
        (err: any, result: any) => {
          // console.log('result', result[0][0]);
          // console.log('err', err);
          if (err) {
            reject(err);
          } else {
            if (result && result[0]?.length > 0) {
              resolve(result[0]);
            } else {
              resolve([]);
            }
          }
        }
      );
    } catch (error) {
      reject(error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getConcurrentChat = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_GetConcurrentAgentLimit(?)',
        [
          data?.domainId ?? null
        ],
        (err: any, result: any) => {
          console.log('up_GetConcurrentAgentLimit result', result);
          console.log('up_GetConcurrentAgentLimit err', err);
          if (err) {
            reject(err);
            console.log(err);
          }
          if (result && result.length > 0) {
            resolve(result[0]);
          } else {
            reject(err);
          }
        }
      );
    } catch (err) {
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const insertUpdateConcurrentChat = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_InsertUpdateConcurrentAgentLimit(?,?,?,?,?,?)',
        [
          data?.domainId,
          data?.isAllAgent ?? null,
          data?.allAgentLimit ?? null,
          data?.isAgent ?? null,
          data?.isRoutingProfile ?? null,
          data?.agentOccupyDetails ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            console.log(err);
          }
          if (result && result.length > 0) {
            resolve(result[0]);
          } else {
            reject(err);
          }
        }
      );
    } catch (err) {
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @param dataq
 * @returns
 */
export const agentExtSocketIdDelete = (id: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL DeleteChatConnection(?)',
        [id],
        (err: any, result: any) => {
          if (err) {
            reject(err);
          } else {
            if (result && result[0]?.length > 0) {
              resolve(result[0]);
            } else {
              resolve([]);
            }
          }
        }
      );
    } catch (error) {
      reject(error);
    }
  });
};

/**
 *
 * @param data
 * @param dataq
 * @returns
 */
export const getMaxWaitTime = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call GetMaximumWaitTime(?)',
        [data?.queueId],
        (err: any, result: any) => {
          if (err) {
            reject(err);
          } else {
            if (result && result[0]?.length > 0) {
              resolve(result[0]);
            } else {
              resolve([]);
            }
          }
        }
      );
    } catch (error) {
      reject(error);
    }
  });
};

export const insertNotificationDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('call ccaas_notification_details(?,?,?,?,?,?,?,?,?,?)', [
        data?.uuid,
        data?.domainId,
        data?.toExt,
        data?.notificationMsg,
        data?.markAsRead,
        data?.timeStamp,
        data?.type,
        data?.channelType,
        data?.coid,
        data?.customerValue
      ],                        (err: any, result: any) => {
        console.log('result', result);
        if (err) {
          console.log('err:', err);
          reject(err);
        } else {
          if (result?.length > 0) {
            resolve(result);
          } else {
            resolve([]);
          }
        }
      });
    } catch (err) {
      reject(err);
    }
  });
};

export const getCustomerDetails = (customerNumber: string) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_customer_details_by_phoneNumber(?)',
        [
          customerNumber,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            console.log('getCustomerDetails reject err: ', err);
          } else {
            console.log('getCustomerDetails inside output');
            resolve(result?.length > 0 ? result[0] : null);
          }
        }
      );
    } catch (err) {
      reject(err);
    }
  });
};
export const getCustomerInfo = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_customer_dtl_by_phoneNumber_domainId(?,?)',
        [
          data?.phoneNumber,
          data?.domainId
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            console.log('getCustomerInfo reject err: ', err);
          } else {
            console.log('getCustomerInfo inside output');
            resolve(result?.length > 0 ? result[0] : null);
          }
        }
      );
    } catch (err) {
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @param dataq
 * @returns
 */
export const getUserListByExt = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_getUserListByExtHandler(?,?)',
        [data?.domain_id, data?.ext],
        (err: any, result: any) => {
          if (err) {
            reject(err);
          } else {
            if (result && result[0]?.length > 0) {
              resolve(result[0]);
            } else {
              resolve([]);
            }
          }
        }
      );
    } catch (error) {
      reject(error);
    }
  });
};
