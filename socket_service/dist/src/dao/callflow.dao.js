"use strict";
var __awaiter = (this && this.__awaiter) || function (thisArg, _arguments, P, generator) {
    function adopt(value) { return value instanceof P ? value : new P(function (resolve) { resolve(value); }); }
    return new (P || (P = Promise))(function (resolve, reject) {
        function fulfilled(value) { try { step(generator.next(value)); } catch (e) { reject(e); } }
        function rejected(value) { try { step(generator["throw"](value)); } catch (e) { reject(e); } }
        function step(result) { result.done ? resolve(result.value) : adopt(result.value).then(fulfilled, rejected); }
        step((generator = generator.apply(thisArg, _arguments || [])).next());
    });
};
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
exports.getConcurrentChat = exports.getChatHoursOfOperation = exports.getDomainStatus = exports.findAgentByEmail = exports.getHoursofOperation = exports.getQueueSettings = exports.getJSON = exports.getListChatAgent = exports.getTransferAgentStatus = exports.insertIncomingChatsDB = exports.getIncomingChatDetailsDB = exports.getAgentListChat = exports.getStatusData = exports.insertStatus = exports.getPreviousStatus = exports.getRoutingDetails = exports.getRoutingProfiles = exports.getRoutingProfile = exports.getSessionDetailsDao = exports.updateMessageDao = exports.roleInfoDao = exports.createCustomerFilepath = exports.createCustomerCallBack = exports.getCustomerChatHistory = exports.getConversationList = exports.getSessionAllList = exports.getActivityList = exports.assignCallBackSms = exports.updateCallBackSessionId = exports.assignCallBackCall = exports.assignCallBack = exports.getCallFlowNodes = exports.getRoleIdList = exports.getCallSessionHistory = exports.getCallBackList = exports.getSessionIdDao = exports.getAgentChat = exports.getAgentCall = exports.getHoursofOperationById = exports.callFlowDetailByCfid = exports.callFlowDetails = exports.callFlowList = exports.savecallflow = exports.getAgentList = exports.getAgentListS = exports.updateAgentState = exports.getFiFOQueueS = exports.getQueueSettingS = exports.getnodesdata = exports.callflowhistory = void 0;
exports.insertApiCallCount = exports.getAgentByChatZero = exports.getAgentByChat = exports.getAgentsSkills = exports.getRoutingProfileUser = exports.getConcurrentChatLimit = void 0;
const axios_1 = __importDefault(require("axios"));
const lodash_1 = require("lodash");
const moment_timezone_1 = __importDefault(require("moment-timezone"));
const uuid_1 = require("uuid");
const db_1 = require("../plugins/db");
const callflowhistory = (data, nodeData) => {
    return new Promise((resolve, reject) => __awaiter(void 0, void 0, void 0, function* () {
        try {
            db_1.mysqlPoolConnection.query('call ccaas_call_Flow_history_details(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)', [
                data === null || data === void 0 ? void 0 : data.cfid,
                data === null || data === void 0 ? void 0 : data.templateName,
                nodeData.node,
                nodeData.edge,
                data === null || data === void 0 ? void 0 : data.version,
                data === null || data === void 0 ? void 0 : data.type,
                data === null || data === void 0 ? void 0 : data.createDate,
                data === null || data === void 0 ? void 0 : data.lastModifiedDate,
                data === null || data === void 0 ? void 0 : data.uuid,
                data === null || data === void 0 ? void 0 : data.status,
                data === null || data === void 0 ? void 0 : data.companyId,
                data === null || data === void 0 ? void 0 : data.domainId,
                data === null || data === void 0 ? void 0 : data.callFlowStatus,
                data === null || data === void 0 ? void 0 : data.description,
                data === null || data === void 0 ? void 0 : data.publishedDataArr,
            ], (err, result) => {
                if (err) {
                    reject(err);
                    console.log(err);
                }
                resolve(result);
            });
        }
        catch (err) {
            reject(err);
        }
    }));
};
exports.callflowhistory = callflowhistory;
/**
 *
 * @param sid
 * @returns
 */
const getnodesdata = (cfid) => {
    return new Promise((resolve, reject) => {
        try {
            const mysqlq = `SELECT nodes FROM chatFlowHistory WHERE chatFlowId = '${cfid}'`;
            db_1.mysqlPoolConnection.query(mysqlq, (err, result) => {
                var _a;
                if (err) {
                    reject(err);
                }
                else {
                    resolve((result === null || result === void 0 ? void 0 : result.length) > 0 ? (_a = result === null || result === void 0 ? void 0 : result.at(0)) === null || _a === void 0 ? void 0 : _a.nodes : 'null');
                }
            });
        }
        catch (error) {
            reject(error);
        }
    });
};
exports.getnodesdata = getnodesdata;
/**
 *
 * @param qid
 * @param domainId
 * @returns
 */
const getQueueSettingS = (qid, domainId) => {
    return new Promise((resolve, reject) => {
        try {
            const mysqlqs = `SELECT * FROM queue WHERE qid = ${qid} AND domainId = ${domainId} `;
            db_1.mysqlPoolConnection.query(mysqlqs, (err, results) => {
                if (err) {
                    reject(err);
                }
                else {
                    resolve(results[0]);
                }
            });
        }
        catch (error) {
            reject(error);
        }
    });
};
exports.getQueueSettingS = getQueueSettingS;
/**
 *
 * @param qid
 * @param domainId
 * @returns
 */
/**
 *
 * @param data
 * @returns
 */
const getFiFOQueueS = (data) => {
    try {
        return new Promise((resolve, reject) => __awaiter(void 0, void 0, void 0, function* () {
            const selectQuery = `SELECT * FROM tb_User WHERE emailId ='${data.username}' AND domainId='${data.domainId}'`;
            db_1.mysqlPoolConnection.query(selectQuery, (err, result) => {
                var _a;
                if (err) {
                    reject(err);
                }
                else {
                    const mysqlqs = `SELECT * FROM routingqueueprofilemapping WHERE rqmid = ${(_a = result[0]) === null || _a === void 0 ? void 0 : _a.routing_profile} `;
                    db_1.mysqlPoolConnection.query(mysqlqs, (error, res) => {
                        if (error) {
                            reject(error);
                        }
                        const selectedQueue = res === null || res === void 0 ? void 0 : res.sort((x, y) => {
                            return x.priority - y.priority;
                        });
                        selectedQueue === null || selectedQueue === void 0 ? void 0 : selectedQueue.map((queue) => {
                            const query = `SELECT * FROM queueWeighTime WHERE qid = ${queue.qid}`;
                            db_1.mysqlPoolConnection.query(query, (er, results) => {
                                var _a, _b;
                                if (er) {
                                    reject(er);
                                }
                                else {
                                    if (((_b = (_a = results[0]) === null || _a === void 0 ? void 0 : _a.queueList) === null || _b === void 0 ? void 0 : _b.length) > 0) {
                                        resolve(results);
                                    }
                                }
                            });
                        });
                    });
                }
            });
        }));
    }
    catch (err) {
        console.log(err);
    }
};
exports.getFiFOQueueS = getFiFOQueueS;
/**
 *
 * @param data
 * @returns
 */
const updateAgentState = (data) => {
    return new Promise((resolve, reject) => {
        const StateTimer = Math.floor(Date.now() / 1000);
        try {
            const UpdateState = `UPDATE tb_User SET state= '10', stateTimer=${StateTimer} WHERE domainId = ${data.domainId} AND ext = ${data.ext} `;
            db_1.mysqlPoolConnection.query(UpdateState, (err, Update) => {
                if (err) {
                    reject(err);
                }
                else {
                    resolve(Update);
                }
            });
        }
        catch (error) {
            reject(error);
        }
    });
};
exports.updateAgentState = updateAgentState;
/**
 *
 * @param qid
 * @param domainId
 * @param calledNumber
 * @returns
 */
const getAgentListS = (qid, domainId, calledNumber) => {
    return new Promise((resolve, reject) => {
        try {
            const mysqlqs = `SELECT * FROM routingqueueprofilemapping WHERE qid = ${qid} `;
            db_1.mysqlPoolConnection.query(mysqlqs, (err, results) => {
                if (err) {
                    reject(err);
                }
                else {
                    const rqSetvalue = [];
                    const mapres = results === null || results === void 0 ? void 0 : results.map((data) => rqSetvalue.push(data.rqmid));
                    if (calledNumber !== '436889116888') {
                        const mysqlq = `SELECT * FROM tb_User WHERE routing_profile IN (${rqSetvalue === null || rqSetvalue === void 0 ? void 0 : rqSetvalue.toString()}) AND state = 1 AND domainId = ${domainId} AND ext != 225 `;
                        db_1.mysqlPoolConnection.query(mysqlq, (errors, res) => {
                            const resultArray = res.sort((x, y) => {
                                return x.stateTimer - y.stateTimer;
                            });
                            resolve(resultArray[0]);
                            if (errors) {
                                reject([]);
                            }
                        });
                    }
                    else {
                        const mysqlqso = `SELECT * FROM tb_User WHERE routing_profile IN (${rqSetvalue === null || rqSetvalue === void 0 ? void 0 : rqSetvalue.toString()}) AND state = 1 AND domainId = ${domainId} AND ext = 225`;
                        db_1.mysqlPoolConnection.query(mysqlqso, (erro, result) => {
                            if (erro) {
                                reject([]);
                            }
                            if (result.length > 0) {
                                resolve(result[0]);
                            }
                            else {
                                const mysqlqq = `SELECT * FROM tb_User WHERE routing_profile IN (${rqSetvalue === null || rqSetvalue === void 0 ? void 0 : rqSetvalue.toString()}) AND state = 1 AND domainId = ${domainId} `;
                                db_1.mysqlPoolConnection.query(mysqlqq, (errors, resultss) => {
                                    const resultArray = resultss.sort((x, y) => {
                                        return x.stateTimer - y.stateTimer;
                                    });
                                    resolve(resultArray[0]);
                                    if (errors) {
                                        reject([]);
                                    }
                                });
                            }
                        });
                    }
                }
            });
        }
        catch (error) {
            reject(error);
        }
    });
};
exports.getAgentListS = getAgentListS;
/**
 *
 * @param qid
 * @returns
 */
const getAgentList = (qid) => {
    return new Promise((resolve, reject) => {
        try {
            let domainId = '';
            const mysqlqq = `SELECT * FROM queue WHERE qid = ${qid} `;
            db_1.mysqlPoolConnection.query(mysqlqq, (error, res) => {
                domainId = res[0].domainId;
                const mysqlqs = `SELECT * FROM routingqueueprofilemapping WHERE qid = ${qid} `;
                db_1.mysqlPoolConnection.query(mysqlqs, (err, results) => {
                    var _a;
                    if (err) {
                        reject(err);
                    }
                    else {
                        const mysqlq = `SELECT * FROM tb_User WHERE routing_profile = ${(_a = results[0]) === null || _a === void 0 ? void 0 : _a.rqmid} AND state = 1 AND domainId = ${domainId}`;
                        db_1.mysqlPoolConnection.query(mysqlq, (errors, result) => {
                            const resultArray = result.sort((x, y) => {
                                return x.stateTimer - y.stateTimer;
                            });
                            resolve(resultArray);
                            if (errors) {
                                reject([]);
                            }
                        });
                    }
                });
            });
        }
        catch (error) {
            reject(error);
        }
    });
};
exports.getAgentList = getAgentList;
/**
 * @description Creat call flow
 * @param data
 * @returns
 */
/* export const savecallflow = async (data: any) => {
  try {
    const isAvailable = await callflowModel.find({ cfid: data.cfid });
    if (isAvailable && isAvailable.length > 0) {
      const updateDate: any = {};
      updateDate.templateName = data.templateName;
      updateDate.nodes = data.nodes;
      updateDate.edges = data.edges;
      updateDate.version = data.version;
      updateDate.lastModifiedDate = Date.now();
      updateDate.uuid = data.uuid;
      updateDate.callFlowStatus = data.callFlowStatus;
      updateDate.status = data.status;
      updateDate.description = data.description;
      return await callflowModel
        .updateOne({ cfid: data.cfid }, { $set: updateDate })
        .then((res: any) => res)
        .catch((err: any) => {
          console.log(err);
        });
    }
    return await callflowModel
      .create(data)
      .then((res: any) => res)
      .catch((err: any) => {
        console.log(err);
      });

    // const adata = await callflowModel.find({ cfid: data.cfid }).then(async (res) => {
    //   if (res && res.length > 0) {
    //     const updateDate: any = {};
    //     updateDate.templateName = data.templateName;
    //     updateDate.nodes = data.nodes;
    //     updateDate.edges = data.edges;
    //     updateDate.version = data.version;
    //     updateDate.lastModifiedDate = Date.now();
    //     updateDate.uuid = data.uuid;
    //     updateDate.callFlowStatus = data.callFlowStatus;
    //     callflowModel.updateOne({ cfid: data.cfid }, { $set: updateDate }).then((result) => {
    //       ;
    //       return result;
    //     }).catch((err) => {
    //       ;
    //       return err;
    //     });
    //   } else {
    //     const adat = await callflowModel.create(data).then((result)=>{

    //       ;
    //       return result;
    //     }).catch((e)=>{
    //       ;
    //     });
    //     // return true;
    //     ;
    //   }
    // }).catch((err1) => {
    //   ;
    //   return err1;
    // });
    // ;
  } catch (err2) {
    return err2;
  }
}; */
/**
 *  savecallflow
 */
const savecallflow = (data, nodeData) => {
    console.log('data?.callFlowId----', data === null || data === void 0 ? void 0 : data.callFlowId);
    return new Promise((resolve, reject) => __awaiter(void 0, void 0, void 0, function* () {
        var _a;
        try {
            db_1.mysqlPoolConnection.query('call ccaas_call_Flow_details(?,?,?,?,?,?,?,?,?,?,?,?,?,?)', [
                data === null || data === void 0 ? void 0 : data.cfid,
                data === null || data === void 0 ? void 0 : data.templateName,
                nodeData === null || nodeData === void 0 ? void 0 : nodeData.node,
                nodeData === null || nodeData === void 0 ? void 0 : nodeData.edge,
                data === null || data === void 0 ? void 0 : data.version,
                data === null || data === void 0 ? void 0 : data.type,
                data === null || data === void 0 ? void 0 : data.createDate,
                data === null || data === void 0 ? void 0 : data.lastModifiedDate,
                data === null || data === void 0 ? void 0 : data.status,
                data === null || data === void 0 ? void 0 : data.companyId,
                data === null || data === void 0 ? void 0 : data.domainId,
                data === null || data === void 0 ? void 0 : data.description,
                data === null || data === void 0 ? void 0 : data.publishedDataArr,
                (_a = data === null || data === void 0 ? void 0 : data.callFlowId) !== null && _a !== void 0 ? _a : 0
            ], (err, result) => {
                if (err) {
                    reject(err);
                    console.log(err);
                }
                resolve(result);
            });
        }
        catch (err) {
            reject(err);
        }
    }));
};
exports.savecallflow = savecallflow;
/**
 * @description Get Call flow list
 * @param data
 * @returns
 */
/* export const callFlowList = async (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      callflowModel
        .find({ domainId: data.domainId }, { _id: 0, __v: 0 })
        .sort({ lastModifiedDate: -1 })
        .then((res) => {
          resolve(res);
        })
        .catch((err) => {
          reject(err);
        });
    } catch (err) {
      reject(err);
    }
  });
}; */
/**
 * @description Get Call flow list
 * @param data
 * @returns
 */
const callFlowList = (data) => {
    return new Promise((resolve, reject) => {
        try {
            // const mysqlq = `SELECT * FROM callFlow where domainId =${data.domainId} order by cid desc`;
            // mysqlPoolConnection.query(mysqlq, (err, result) => {
            //   if (err) {
            //     reject(err);
            //   } else {
            //     resolve(result);
            //   }
            // });
            db_1.mysqlPoolConnection.query('call callFlowList(?)', [data.domainId], (err, result) => {
                if (err) {
                    reject(err);
                }
                resolve((result === null || result === void 0 ? void 0 : result.length) ? result[0] : {});
            });
        }
        catch (error) {
            reject(error);
        }
    });
};
exports.callFlowList = callFlowList;
/**
 * @description Get Call flow Details
 * @param data
 * @returns
 */
const callFlowDetails = (data) => {
    return new Promise((resolve, reject) => {
        try {
            const mysqlq = `SELECT * FROM callFlow where domainId = ${data.domainId} AND cfid = '${data.cfid}'`;
            db_1.mysqlPoolConnection.query(mysqlq, (err, result) => {
                if (err) {
                    reject(err);
                }
                else {
                    resolve(result);
                }
            });
        }
        catch (error) {
            reject(error);
        }
    });
};
exports.callFlowDetails = callFlowDetails;
/**
 * @description Get Call flow Details by cfid
 * @param data
 * @returns
 */
const callFlowDetailByCfid = (data) => {
    return new Promise((resolve, reject) => {
        try {
            const mysqlq = `SELECT * FROM callFlowHistory where domainId = ${data.domainId} AND cfid = '${data.cfid}'`;
            db_1.mysqlPoolConnection.query(mysqlq, (err, result) => {
                if (err) {
                    reject(err);
                }
                else {
                    resolve(result);
                }
            });
        }
        catch (error) {
            reject(error);
        }
    });
};
exports.callFlowDetailByCfid = callFlowDetailByCfid;
/**
 * @description Get Call flow Details
 * @param data
 * @returns
 */
/* export const callFlowDetails = async (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      callflowhistoryModel
        .find({ cfid: data.cfid }, { _id: 0 })
        .sort({ createDate: -1 })
        .then((res) => {
          resolve(res);
        })
        .catch((err) => {
          reject(err);
        });
    } catch (err) {
      reject(err);
    }
  });
}; */
/**
 * @description Get Call flow list
 * @param data
 * @returns
 */
/* export const getAllActive = async (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      let channelType: any;
      if (data.channelType === 'All') {
        channelType = { $in: ['Chat', 'Call'] };
      } else {
        channelType = data.channelType;
      }
      let offset;
      if (data.offset > 0) {
        offset = data.offset * 10;
      } else {
        offset = data.offset;
      }
      sessionHistoryModel
        .find({
          $and: [
            {
              channelType,
              domainId: data.domainId,
              sessionStartTime: {
                $gte: data.fromDate,
                $lte: data.toDate,
              },
              $or: [
                {
                  customerDetails: { $regex: new RegExp('.*' + `${data.search}` + '.*', 'i') },
                },
                {
                  callType: { $regex: new RegExp('.*' + `${data.search}` + '.*', 'i') },
                },
                {
                  callDuration: { $regex: new RegExp('.*' + `${data.search}` + '.*', 'i') },
                },
                {
                  summary: { $regex: new RegExp('.*' + `${data.search}` + '.*', 'i') },
                },
                {
                  disposition: { $regex: new RegExp('.*' + `${data.search}` + '.*', 'i') },
                },
              ],
              agentDetails: { $in: data.ext },
            },
          ],
        })
        .sort({ _id: -1 })
        .skip(offset)
        .limit(data.limit)
        .then((res) => {
          resolve(res);
        })
        .catch((err) => {
          reject(err);
        });
    } catch (err) {
      reject(err);
    }
  });
};
 */
/**
 * cid
 */
const getHoursofOperationById = (cid) => {
    return new Promise((resolve, reject) => {
        try {
            const mysqlqs = `SELECT * FROM custom_hours WHERE cid = ${cid !== null && cid !== void 0 ? cid : null} `;
            db_1.mysqlPoolConnection.query(mysqlqs, (error, hoursResult) => {
                var _a, _b, _c, _d, _e, _f, _g, _h, _j, _k, _l;
                if (error) {
                    console.log('error', error);
                }
                else {
                    try {
                        const hours = JSON.parse((_b = (_a = hoursResult[0]) === null || _a === void 0 ? void 0 : _a.hours) !== null && _b !== void 0 ? _b : 'null');
                        // let timeZone:any = hoursResult[0]?.timeZone.split(" ")[0];
                        const timeZone = (_c = hoursResult[0]) === null || _c === void 0 ? void 0 : _c.timeZoneValue;
                        const Currentday = (_e = (_d = (0, moment_timezone_1.default)()) === null || _d === void 0 ? void 0 : _d.tz(timeZone)) === null || _e === void 0 ? void 0 : _e.format('dddd');
                        const CurrentDayHours = hours === null || hours === void 0 ? void 0 : hours.filter((item) => {
                            if (item.day === Currentday) {
                                return;
                                item;
                            }
                        });
                        const currentTime = (_g = (_f = (0, moment_timezone_1.default)(new Date())) === null || _f === void 0 ? void 0 : _f.tz(timeZone)) === null || _g === void 0 ? void 0 : _g.format('HH:mm');
                        const timeArray = [];
                        for (const i of (_j = (_h = CurrentDayHours[0]) === null || _h === void 0 ? void 0 : _h.entry) !== null && _j !== void 0 ? _j : []) {
                            const startTime = (0, moment_timezone_1.default)(new Date(parseInt(i === null || i === void 0 ? void 0 : i.startTimeStamp, 10)))
                                .tz('Asia/Kolkata')
                                .format('HH:mm');
                            const endTime = (0, moment_timezone_1.default)(new Date(parseInt(i === null || i === void 0 ? void 0 : i.endTimeStamp, 10)))
                                .tz('Asia/Kolkata')
                                .format('HH:mm');
                            timeArray.push([startTime, endTime]);
                        }
                        for (const time of timeArray !== null && timeArray !== void 0 ? timeArray : []) {
                            const [fromHour, fromMinute] = (_k = time[0]) === null || _k === void 0 ? void 0 : _k.split(':');
                            const [toHour, toMinute] = (_l = time[1]) === null || _l === void 0 ? void 0 : _l.split(':');
                            const [nowHour, nowMinute] = currentTime === null || currentTime === void 0 ? void 0 : currentTime.split(':');
                            const fromTimestamp = parseInt(fromHour, 10) * 3600000 + parseInt(fromMinute, 10) * 60000;
                            const toTimestamp = parseInt(toHour, 10) * 3600000 + parseInt(toMinute, 10) * 60000;
                            const nowTimestamp = parseInt(nowHour, 10) * 3600000 + parseInt(nowMinute, 10) * 60000;
                            if (nowTimestamp >= fromTimestamp && nowTimestamp <= toTimestamp) {
                                resolve(true);
                            }
                        }
                        resolve(false);
                    }
                    catch (error) {
                        console.log(error);
                    }
                }
            });
        }
        catch (error) {
            console.log(error);
            reject(error);
        }
    });
};
exports.getHoursofOperationById = getHoursofOperationById;
/**
 * total Voice call count
 * @param data
 * @returns
 */
/* export const getAllActivityCount = async (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      sessionHistoryModel
        .find({
          $and: [
            {
              domainId: data.domainId,
              sessionStartTime: {
                $gte: data.fromDate,
                $lte: data.toDate,
              },
              agentDetails: { $in: data.ext },
            },
          ],
        })
        .count()
        .then((res) => {
          resolve(res);
        })
        .catch((err) => {
          reject(err);
        });
    } catch (err) {
      reject(err);
    }
  });
}; */
/**
 * @description Get Call flow list
 * @param data
 * @returns
 */
/* export const getAgentCall = async (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      //
      sessionHistoryModel
        .find({ agentDetails: { $in: data }, channelType: 'Call' })
        .sort({ _id: -1 })
        .then((res) => {
          resolve(res);
        })
        .catch((err) => {
          reject(err);
        });
    } catch (err) {
      reject(err);
    }
  });
}; */
/**
 * @description Get Call flow list
 * @param data
 * @returns
 */
const getAgentCall = (data) => __awaiter(void 0, void 0, void 0, function* () {
    return new Promise((resolve, reject) => __awaiter(void 0, void 0, void 0, function* () {
        try {
            const res = yield db_1.mysqlPoolConnection.query("SELECT * FROM sessionHistory WHERE agentDetails IN (?) AND channelType = 'Call' ORDER BY shid DESC", [data]);
            resolve(res);
        }
        catch (err) {
            reject(err);
        }
    }));
});
exports.getAgentCall = getAgentCall;
/**
 * @description Get Call flow list
 * @param data
 * @returns
 */
/* export const getAgentChat = async (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      //
      sessionHistoryModel
        .find({ agentDetails: { $in: data }, channelType: 'Chat' })
        .sort({ _id: -1 })
        .then((res) => {
          resolve(res);
        })
        .catch((err) => {
          reject(err);
        });
    } catch (err) {
      reject(err);
    }
  });
}; */
/**
 * @description Get Call flow list
 * @param data
 * @returns
 */
const getAgentChat = (data) => __awaiter(void 0, void 0, void 0, function* () {
    return new Promise((resolve, reject) => __awaiter(void 0, void 0, void 0, function* () {
        try {
            const res = yield db_1.mysqlPoolConnection.query("SELECT * FROM sessionHistory WHERE agentDetails IN (?) AND channelType = 'Chat' ORDER BY shid DESC", [data]);
            resolve(res);
        }
        catch (err) {
            reject(err);
        }
    }));
});
exports.getAgentChat = getAgentChat;
/**
 * @description Get sessionId flow list
 * @param data
 * @returns
 */
/* export const getSessionIdDao = async (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      conversationHistoryModel
        .findOne({ sessionId: data.sessionId })
        .then((res) => {
          resolve(res);
        })
        .catch((err) => {
          reject(err);
        });
    } catch (err) {
      reject(err);
    }
  });
}; */
/**
 * @description Get sessionId flow list
 * @param data
 * @returns
 */
const getSessionIdDao = (sessionId) => {
    return new Promise((resolve, reject) => {
        try {
            const mysqlq = `SELECT * FROM conversationHistory WHERE sessionId = '${sessionId}'`;
            db_1.mysqlPoolConnection.query(mysqlq, (err, result) => {
                if (err) {
                    reject(err);
                }
                else {
                    resolve(result);
                }
            });
        }
        catch (error) {
            reject(error);
        }
    });
};
exports.getSessionIdDao = getSessionIdDao;
/**
 * @description Get call back list
 * @param data
 * @returns
 */
/* export const getCallBackList = async (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      let channelType;
      if (data.channelType === 'All') {
        channelType = { $in: ['Chat', 'Call', 'Email', 'Sms'] };
      } else {
        channelType = data.channelType;
      }
      let offset;
      if (data.offset > 0) {
        offset = data.offset * 10;
      } else {
        offset = data.offset;
      }
      sessionHistoryModel
        .find({
          $and: [
            {
              channelType,
              domainId: data.domainId,
              sessionStartTime: {
                $gte: data.fromDate,
                $lte: data.toDate,
              },
              agentDetails: { $in: data.ext },
              disposition: { $in: ['missed', 'abandoned'] },
            },
            {
              $or: [
                {
                  'customerData.FirstName': { $regex: new RegExp('.*' + `${data.search}` + '.*', 'i') },
                },
                {
                  'customerData.LastName': { $regex: new RegExp('.*' + `${data.search}` + '.*', 'i') },
                },
                {
                  'customerData.MobileNo': { $regex: new RegExp('.*' + `${data.search}` + '.*', 'i') },
                },
                {
                  'customerData.Email': { $regex: new RegExp('.*' + `${data.search}` + '.*', 'i') },
                },
                {
                  queue: { $regex: new RegExp('.*' + `${data.search}` + '.*', 'i') },
                },
                {
                  callType: { $regex: new RegExp('.*' + `${data.search}` + '.*', 'i') },
                },
                {
                  disposition: { $regex: new RegExp('.*' + `${data.search}` + '.*', 'i') },
                },
              ],
            },
          ],
        })
        .sort({ _id: -1 })
        .skip(offset)
        .limit(data.limit)
        .then((res) => {
          resolve(res);
        })
        .catch((err) => {
          reject(err);
        });
    } catch (err) {
      reject(err);
    }
  });
}; */
/**
 * get CallBack List
 * @param data
 * @returns
 */
const getCallBackList = (data) => {
    // console.log('dataa getCallBackList: ', data);
    return new Promise((resolve, reject) => __awaiter(void 0, void 0, void 0, function* () {
        var _a;
        try {
            db_1.mysqlPoolConnection.query('call ccass_get_callback_session_history_details(?,?,?,?,?,?,?,?,?)', [
                data === null || data === void 0 ? void 0 : data.domainId,
                data === null || data === void 0 ? void 0 : data.channelType,
                data === null || data === void 0 ? void 0 : data.fromDate,
                data === null || data === void 0 ? void 0 : data.toDate,
                '',
                (_a = data === null || data === void 0 ? void 0 : data.userID) !== null && _a !== void 0 ? _a : null,
                data === null || data === void 0 ? void 0 : data.search,
                data === null || data === void 0 ? void 0 : data.offset,
                data === null || data === void 0 ? void 0 : data.limit
            ], (err, result) => {
                if (err) {
                    reject(err);
                }
                resolve((result === null || result === void 0 ? void 0 : result.length) ? result[0] : {});
            });
        }
        catch (err) {
            reject(err);
        }
    }));
};
exports.getCallBackList = getCallBackList;
/**
 *
 * @param data
 * @returns
 */
const getCallSessionHistory = (data) => {
    return new Promise((resolve, reject) => __awaiter(void 0, void 0, void 0, function* () {
        var _a, _b, _c, _d, _e, _f, _g;
        try {
            db_1.mysqlPoolConnection.query('call up_GetSessionHistoryDetails(?,?,?,?,?,?,?,?,?)', [
                data === null || data === void 0 ? void 0 : data.domainId,
                data === null || data === void 0 ? void 0 : data.channelType,
                (_a = data === null || data === void 0 ? void 0 : data.dispositionId) !== null && _a !== void 0 ? _a : null,
                (_b = data === null || data === void 0 ? void 0 : data.isInbound) !== null && _b !== void 0 ? _b : null,
                (_c = data === null || data === void 0 ? void 0 : data.fromDate) !== null && _c !== void 0 ? _c : null,
                (_d = data === null || data === void 0 ? void 0 : data.toDate) !== null && _d !== void 0 ? _d : null,
                (_e = data === null || data === void 0 ? void 0 : data.userID) !== null && _e !== void 0 ? _e : null,
                (_f = data === null || data === void 0 ? void 0 : data.offset) !== null && _f !== void 0 ? _f : null,
                (_g = data === null || data === void 0 ? void 0 : data.limit) !== null && _g !== void 0 ? _g : null,
            ], (err, result) => {
                if (err) {
                    reject(err);
                }
                resolve((result === null || result === void 0 ? void 0 : result.length) ? result[0] : []);
            });
        }
        catch (err) {
            reject(err);
        }
    }));
};
exports.getCallSessionHistory = getCallSessionHistory;
/**
 * @description Get call back list
 * @param data
 * @returns
 */
/* export const getCallBackCount = async (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      sessionHistoryModel
        .find({
          $and: [
            {
              domainId: data.domainId,
              sessionStartTime: {
                $gte: data.fromDate,
                $lte: data.toDate,
              },
              agentDetails: { $in: data.ext },
              disposition: { $in: ['missed', 'abandoned'] },
            },
          ],
        })
        .count()
        .then((res) => {
          resolve(res);
        })
        .catch((err) => {
          reject(err);
        });
    } catch (err) {
      reject(err);
    }
  });
}; */
/**
 * get voice call list
 * @param data
 * @returns
 */
/* export const getVoiceCallList = async (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      let channelType;
      if (data.channelType === 'All') {
        channelType = { $in: ['Chat', 'Call', 'Email', 'Sms'] };
      } else {
        channelType = data.channelType;
      }
      let disposition;
      if (data.disposition === 'All') {
        disposition = { $in: ['Resolved', 'Unresolved', 'Transferred', 'Closed', 'Follow-Up', 'DoNotContact'] };
      } else {
        disposition = data.disposition;
      }
      let callType;
      if (data.callType === 'All') {
        callType = { $in: ['inbound', 'outbound'] };
      } else {
        callType = data.callType;
      }
      let offset;
      if (data.offset > 0) {
        offset = data.offset * 10;
      } else {
        offset = data.offset;
      }
      sessionHistoryModel
        .find({
          $and: [
            {
              channelType,
              disposition,
              callType,
              domainId: data.domainId,
              sessionStartTime: {
                $gte: data.fromDate,
                $lte: data.toDate,
              },
              agentDetails: { $in: data.ext },
            },
            {
              $or: [
                {
                  'customerData.FirstName': { $regex: new RegExp('.*' + `${data.search}` + '.*', 'i') },
                },
                {
                  'customerData.LastName': { $regex: new RegExp('.*' + `${data.search}` + '.*', 'i') },
                },
                {
                  'customerData.MobileNo': { $regex: new RegExp('.*' + `${data.search}` + '.*', 'i') },
                },
                {
                  'customerData.Email': { $regex: new RegExp('.*' + `${data.search}` + '.*', 'i') },
                },
                {
                  queue: { $regex: new RegExp('.*' + `${data.search}` + '.*', 'i') },
                },
                {
                  callType: { $regex: new RegExp('.*' + `${data.search}` + '.*', 'i') },
                },
                {
                  disposition: { $regex: new RegExp('.*' + `${data.search}` + '.*', 'i') },
                },
              ],
            },
          ],
        })
        .sort({ _id: -1 })
        .skip(offset)
        .limit(data.limit)
        .then((res) => {
          resolve(res);
        })
        .catch((err) => {
          reject(err);
        });
    } catch (err) {
      reject(err);
    }
  });
};
 */
// export const getRoleIdList = (data: any) => {
//   return new Promise((resolve, reject) => {
//     try {
//       if (data.emailId) {
//         const mysqlqs = `SELECT ext FROM tb_User WHERE report_to = '${data.emailId}'`;
//         mysqlPoolConnection.query(mysqlqs, (err, results) => {
//           if (err) {
//             reject(err);
//           } else {
//             resolve(results);
//           }
//         });
//       }
//     } catch (error) {
//       reject(error);
//     }
//   });
// };
/**
 *
 * @param data
 * @returns
 */
const getRoleIdList = (data) => {
    return new Promise((resolve, reject) => {
        try {
            if (data === null || data === void 0 ? void 0 : data.emailId) {
                db_1.mysqlPoolConnection.query('call getRoleIdList(?)', [
                    data === null || data === void 0 ? void 0 : data.emailId
                ], (err, result) => {
                    if (err) {
                        reject(err);
                    }
                    resolve(result[0]);
                });
            }
        }
        catch (error) {
            reject(error);
        }
    });
};
exports.getRoleIdList = getRoleIdList;
const getCallFlowNodes = (data) => {
    return new Promise((resolve, reject) => {
        try {
            const mysqlqs = `SELECT * FROM callFlow WHERE cfid = '${data.cfid}'`;
            db_1.mysqlPoolConnection.query(mysqlqs, (err, results) => {
                if (err) {
                    reject(err);
                }
                else {
                    resolve(results);
                }
            });
        }
        catch (error) {
            reject(error);
        }
    });
};
exports.getCallFlowNodes = getCallFlowNodes;
/**
 * total Voice call count
 * @param data
 * @returns
 */
/* export const getVoiceListCount = async (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      sessionHistoryModel
        .find({
          $and: [
            {
              domainId: data.domainId,
              sessionStartTime: {
                $gte: data.fromDate,
                $lte: data.toDate,
              },
              agentDetails: { $in: data.ext },
            },
          ],
        })
        .countDocuments()
        .then((res) => {
          resolve(res);
        })
        .catch((err) => {
          reject(err);
        });
    } catch (err) {
      reject(err);
    }
  });
}; */
/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */
/* export const assignCallBack = async (data: any) => {
  try {
    return new Promise(async (resolve, reject) => {
      sessionHistoryModel
        .updateOne(
          { sessionId: data.sessionId, agentDetails: data.ext, domainId: data.domainId },
          { $set: { assignedTo: data.assignedTo, assignedBy: data.assignedBy, assigned: true } }
        )
        .then((res: any) => {
          const data1 = {
            id: uuidv4(),
            type: 'Call back assigned by supervisor',
            message: `Callback assigned by ${data.assignedBy} Callback to <Customer Name or Mobile Number> ${data.sessionId}`,
          };
          notificationMessages(data1, data.ext);
          insertNotificationDao({
            uuid: data1.id,
            domainId: data.domainId,
            toExt: data.ext,
            notificationMsg: data1.message,
            type: data1.type,
            channelType: 'call',
            markAsRead: 1,
            timeStamp: new Date().getTime(),
          });
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
 * @returns
 */
/* export const assignCallBack = async (data: any) => {
  try {
    return new Promise(async (resolve, reject) => {
      // const res = await mysqlPoolConnection.query(
      //   'UPDATE sessionHistory SET assignedTo = ?, assignedBy = ?, assigned = ? assignedOn = ? WHERE sessionId = ? AND agentDetails = ? AND domainId = ?',
      //   [data.assignedTo, data.assignedBy, 0, data.assignedOn,  data.sessionId, data.ext, data.domainId]
      // );
      const assignToData = JSON.stringify(data?.assignedTo);
      console.log(data, '----------');
      // mysqlPoolConnection.query(
      //   'call ccaas_session_history_details(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,??,?,?,?,?,?,?,?,?,?,?,?,?,?)',
      //   [
      //     data?.session_id,
      //     data?.sessionStartTime ?? null,
      //     data?.sessionEndTime ?? null,
      //     data?.botStartTime ?? null,
      //     data?.botEndTime ?? null,
      //     data?.agentConnectTime ?? null,
      //     data?.agentDisconnectTime ?? null,
      //     data?.ipAddress ?? null,
      //     data?.source ?? null,
      //     data?.lastMessage ?? null,
      //     data?.botDetails ?? null,
      //     data?.category ?? null,
      //     true,
      //     data?.agentComments ?? null,
      //     data?.agentDetails ?? null,
      //     data?.cli ?? null,
      //     data?.browser ?? null,
      //     data?.OS ?? null,
      //     true,
      //     data?.skillsIdentify ?? null,
      //     data?.callFlowSourceId ?? null,
      //     data?.channelType,
      //     data?.location ?? null,
      //     data?.deviceType ?? null,
      //     data?.customerData ?? null,
      //     data?.domainId ?? null,
      //     data?.companyId ?? null,
      //     data?.disposition ?? null,
      //     data?.status ?? null,
      //     data?.priority ?? null,
      //     data?.summary ?? null,
      //     data?.follow_up_action ?? null,
      //     data?.recordingUrl ?? null,
      //     data?.callType ?? null,
      //     data?.queue ?? null,
      //     data?.wrapTime ?? null,
      //     data?.botDuration ?? null,
      //     data?.agentDuration ?? null,
      //     data?.agentRecordingUrl ?? null,
      //     data?.disconnectedBy ?? null,
      //     data?.qid ?? null,
      //     data?.assigned ?? null,
      //     assignToData ?? null,
      //     data?.assignedBy ?? null,
      //     data?.waitDuration ?? null,
      //     data?.agentHoldDuration ?? null,
      //     data?.dailDuration ?? null,
      //     data?.calledNumber ?? null,
      //     data?.queueDuration ?? null,
      //     data?.queueName ?? null,
      //     data?.callDuration ?? null,
      //     data.message ?? null,
      //     data?.customerFile ?? null,
      //     data?.agentName ?? null,
      //     data?.skillName ?? null,
      //     data?.dispositionName ?? null,
      //     data?.afterCallWorkTime ?? null,
      //     data?.callRecorded === 1 ? 1 : 0 ?? null, // Recording type: If call is recorded or not
      //     data?.callDuration ?? null, // Recording duration: Same as call duration
      //     data?.transferred ?? null,
      //     data?.activeChatTime ?? null,
      //     data?.responseTime ?? null,
      //     data?.assignedOn ?? null,
      //     data?.chatSession ?? null,
      //     data?.transferredTo ?? null,
      //     data?.markAsRead ?? null,
      //     data?.agentStatus ?? null,
      //     data?.calledPersonDetails ?? null,
      //     data?.subject ?? null,
      //     data?.primarySkill ?? null,
      //     data?.primaryAgent ?? null,
      //     data?.transferredSkill ?? null,
      //     data?.transferredAgent ?? null,
      //     data?.primaryQueue ?? null,
      //     data?.transferredQueue ?? null,
      //     data?.transferredTime ?? null,
      //     data?.transferredType ?? null,
      //     data?.voicemailUrl ?? null,
      //     data?.vmsTranscript ?? null,
      //     data?.callBackDetails ?? null,
      //     data?.supervisorName ?? null,
      //   ],
      //   (err: any, result: any) => {
      //     if (err) {
      //       reject(err);
      //       console.log(err);
      //     }
      //     resolve(result);
      //   }
      // );
      const query = `UPDATE sessionHistory SET assignedTo='${assignToData}', assignedBy='${data?.assignedBy}', assignedOn='${data?.assignedOn}' WHERE sessionId='${data.session_id}' AND domainId=${data.domainId}`;
      mysqlPoolConnection.query(query, (err: any, results: any) => {
        if (err) {
          reject(err);
        } else {
          resolve(results);
        }
      });
      const data1 = {
        id: uuidv4(),
        type: 'Call back assigned by supervisor',
        message: `Callback assigned by ${data.assignedBy} Callback to <Customer Name or Mobile Number> ${data.sessionId}`,
      };
      notificationMessages(data1, data.ext);
      insertNotificationDao({
        uuid: data1.id,
        domainId: data.domainId,
        toExt: data.ext,
        notificationMsg: data1?.message,
        type: data1.type,
        channelType: 'call',
        markAsRead: 1,
        timeStamp: new Date().getTime(),
      });
    });
  } catch (err) {
    console.log(err);
  }
}; */
/**
 * get activity list
 * @param data
 * @returns
 */
/* export const getActivityList = async (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      let channelType;
      if (data.channelType === 'All') {
        channelType = { $in: ['Chat', 'Call', 'Email', 'Sms'] };
      } else {
        channelType = data.channelType;
      }
      let disposition;
      if (data.disposition === 'All') {
        disposition = { $in: ['Resolved', 'Unresolved', 'Transferred', 'Closed', 'Follow-up', 'Do Not Contact'] };
      } else {
        disposition = data.disposition;
      }
      let callType;
      if (data.callType === 'All') {
        callType = { $in: ['inbound', 'outbound'] };
      } else {
        callType = data.callType;
      }
      let offset;
      if (data.offset > 0) {
        offset = data.offset * 10;
      } else {
        offset = data.offset;
      }
      sessionHistoryModel
        .find({
          $and: [
            {
              channelType,
              disposition,
              callType,
              domainId: data.domainId,
              sessionStartTime: {
                $gte: data.fromDate,
                $lte: data.toDate,
              },
              agentDetails: { $in: data.ext },
            },
            {
              $or: [
                {
                  'customerData.FirstName': { $regex: new RegExp('.*' + `${data.search}` + '.*', 'i') },
                },
                {
                  'customerData.LastName': { $regex: new RegExp('.*' + `${data.search}` + '.*', 'i') },
                },
                {
                  'customerData.MobileNo': { $regex: new RegExp('.*' + `${data.search}` + '.*', 'i') },
                },
                {
                  'customerData.Email': { $regex: new RegExp('.*' + `${data.search}` + '.*', 'i') },
                },
                {
                  queue: { $regex: new RegExp('.*' + `${data.search}` + '.*', 'i') },
                },
                {
                  callType: { $regex: new RegExp('.*' + `${data.search}` + '.*', 'i') },
                },
                {
                  disposition: { $regex: new RegExp('.*' + `${data.search}` + '.*', 'i') },
                },
              ],
            },
          ],
        })
        .sort({ _id: -1 })
        .skip(offset)
        .limit(data.limit)
        .then((res) => {
          resolve(res);
        })
        .catch((err) => {
          reject(err);
        });
    } catch (err) {
      reject(err);
    }
  });
}; */
/**
 *
 * @param qdata
 * @returns
 */
const assignCallBack = (data) => {
    return new Promise((resolve, reject) => {
        var _a, _b, _c, _d, _e;
        try {
            const sessionId = data.Istransferred === 1 ? (0, uuid_1.v4)() : null;
            db_1.mysqlPoolConnection.query('call up_UpdateAssignAgentToEmail(?,?,?,?,?,?)', [
                (_a = data.assignedBy) !== null && _a !== void 0 ? _a : null,
                (_b = data.assignedTo) !== null && _b !== void 0 ? _b : null,
                (_c = data.assignedOn) !== null && _c !== void 0 ? _c : null,
                (_d = data.EmailHistoryId) !== null && _d !== void 0 ? _d : null,
                (_e = data.Istransferred) !== null && _e !== void 0 ? _e : null,
                sessionId !== null && sessionId !== void 0 ? sessionId : null,
            ], (err, result) => {
                if (err) {
                    console.log(err);
                    reject(err);
                }
                resolve(result);
            });
        }
        catch (error) {
            reject(error);
        }
    });
};
exports.assignCallBack = assignCallBack;
const assignCallBackCall = (data) => {
    return new Promise((resolve, reject) => {
        try {
            db_1.mysqlPoolConnection.query('call up_UpdateAssignAgentToCall(?,?,?,?)', [
                data.assignedBy,
                data.assignedTo,
                data.assignedOn,
                data.CallHistoryId,
            ], (err, result) => {
                if (err) {
                    console.log(err);
                    reject(err);
                }
                resolve((result === null || result === void 0 ? void 0 : result.length) > 0 ? result[0] : []);
            });
        }
        catch (error) {
            reject(error);
        }
    });
};
exports.assignCallBackCall = assignCallBackCall;
const updateCallBackSessionId = (data) => {
    return new Promise((resolve, reject) => {
        try {
            db_1.mysqlPoolConnection.query('call up_UpdateCallBackCallHistoryId(?,?)', [
                data.CallHistoryId,
                data.callBackSessionId
            ], (err, result) => {
                if (err) {
                    console.log(err);
                    reject(err);
                }
                resolve((result === null || result === void 0 ? void 0 : result.length) > 0 ? result[0] : []);
            });
        }
        catch (error) {
            reject(error);
        }
    });
};
exports.updateCallBackSessionId = updateCallBackSessionId;
/**
 *
 * @param qdata
 * @returns
 */
const assignCallBackSms = (data) => {
    return new Promise((resolve, reject) => {
        var _a, _b, _c, _d, _e;
        try {
            const sessionId = data.Istransferred === 1 ? (0, uuid_1.v4)() : null;
            db_1.mysqlPoolConnection.query('call up_UpdateAssignAgentToSMS(?,?,?,?,?,?)', [
                (_a = data.assignedBy) !== null && _a !== void 0 ? _a : null,
                (_b = data.assignedTo) !== null && _b !== void 0 ? _b : null,
                (_c = data.assignedOn) !== null && _c !== void 0 ? _c : null,
                (_d = data.SMSHistoryId) !== null && _d !== void 0 ? _d : null,
                (_e = data.Istransferred) !== null && _e !== void 0 ? _e : null,
                sessionId !== null && sessionId !== void 0 ? sessionId : null,
            ], (err, result) => {
                if (err) {
                    console.log(err);
                    reject(err);
                }
                resolve(result);
            });
        }
        catch (error) {
            reject(error);
        }
    });
};
exports.assignCallBackSms = assignCallBackSms;
/**
 * get activity list
 * @param data
 * @returns
 */
const getActivityList = (data) => {
    return new Promise((resolve, reject) => __awaiter(void 0, void 0, void 0, function* () {
        const resultExt = data.ext.toString();
        if (data.channelType === 'Chat,Call,Email') {
            data.channelType = 'All';
        }
        try {
            db_1.mysqlPoolConnection.query('call ccass_get_session_history_details(?,?,?,?,?,?,?,?,?,?)', [data.domainId,
                data.channelType,
                data.disposition === 'All' ? 0 : data.disposition,
                data.callType,
                data.fromDate,
                data.toDate,
                resultExt,
                data.search,
                data.offset,
                data.limit], (err, result) => {
                if (err) {
                    reject(err);
                }
                resolve((result === null || result === void 0 ? void 0 : result.length) ? result[0] : {});
            });
        }
        catch (err) {
            reject(err);
        }
    }));
};
exports.getActivityList = getActivityList;
/**
 * @description Get voice call back list
 * @param data
 * @returns
 */
/* export const getVoiceCallBackList = async (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      let offset;
      if (data.offset > 0) {
        offset = data.offset * 10;
      } else {
        offset = data.offset;
      }
      sessionHistoryModel
        .find({
          $and: [
            {
              domainId: data.domainId,
              sessionStartTime: {
                $gte: data.fromDate,
                $lte: data.toDate,
              },
              disposition: { $in: ['missed', 'abandoned'] },
            },
          ],
        })
        .sort({ _id: -1 })
        .skip(offset)
        .limit(data.limit)
        .then((res) => {
          resolve(res);
        })
        .catch((err) => {
          reject(err);
        });
    } catch (err) {
      reject(err);
    }
  });
}; */
/**
 * @description Get voice call back list
 * @param data
 * @returns
 */
/*
export const getVoiceCallBackExtList = async (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      let offset;
      if (data.offset > 0) {
        offset = data.offset * 10;
      } else {
        offset = data.offset;
      }
      sessionHistoryModel
        .find({
          $and: [
            {
              domainId: data.domainId,
              sessionStartTime: {
                $gte: data.fromDate,
                $lte: data.toDate,
              },
              agentDetails: { $in: data.ext },
              disposition: { $in: ['missed', 'abandoned'] },
            },
          ],
        })
        .sort({ _id: -1 })
        .skip(offset)
        .limit(data.limit)
        .then((res) => {
          resolve(res);
        })
        .catch((err) => {
          reject(err);
        });
    } catch (err) {
      reject(err);
    }
  });
}; */
/**
 * @description Get voice call back list
 * @param data
 * @returns
 */
/* export const getCallBackExtCount = async (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      sessionHistoryModel
        .find({
          $and: [
            {
              domainId: data.domainId,
              sessionStartTime: {
                $gte: data.fromDate,
                $lte: data.toDate,
              },
              agentDetails: { $in: data.ext },
              disposition: { $in: ['missed', 'abandoned'] },
            },
          ],
        })
        .count()
        .then((res) => {
          resolve(res);
        })
        .catch((err) => {
          reject(err);
        });
    } catch (err) {
      reject(err);
    }
  });
}; */
/**
 * @description Get sessionId list
 * @param data
 * @returns
 */
/* export const getSessionAllList = async (dataq: any, data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      sessionHistoryModel
        .find({ customerDetails: dataq, domainId: data.domainId })
        .then((res) => {
          resolve(res);
        })
        .catch((err) => {
          reject(err);
        });
    } catch (err) {
      reject(err);
    }
  });
}; */
/**
 * @description Get sessionId list
 * @param data
 * @returns
 */
const getSessionAllList = (dataq, data) => {
    return new Promise((resolve, reject) => {
        try {
            const mysqlq = `select * from sessionHistory where customerDetails = '${dataq}' AND domainId = ${data.domainId}`;
            db_1.mysqlPoolConnection.query(mysqlq, (err, result) => {
                if (err) {
                    reject(err);
                }
                else {
                    resolve(result);
                }
            });
        }
        catch (error) {
            reject(error);
        }
    });
};
exports.getSessionAllList = getSessionAllList;
/**
 * @description Get sessionId list
 * @param data
 * @returns
 */
/* export const getConversationList = async (sessionId: any, data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      conversationHistoryModel
        .find({ sessionId, domainId: data.domainId })
        .then((res) => {
          resolve(res);
        })
        .catch((err) => {
          reject(err);
        });
    } catch (err) {
      reject(err);
    }
  });
}; */
/**
 * @description get Conversation List
 * @param data
 * @returns
 */
const getConversationList = (sessionId, data) => {
    return new Promise((resolve, reject) => {
        try {
            const mysqlq = `select * from conversationHistory where sessionId = '${sessionId}' AND domainId = ${data.domainId}`;
            db_1.mysqlPoolConnection.query(mysqlq, (err, result) => {
                if (err) {
                    reject(err);
                }
                else {
                    resolve(result);
                }
            });
        }
        catch (error) {
            reject(error);
        }
    });
};
exports.getConversationList = getConversationList;
const getCustomerChatHistory = (ChatHistoryId) => {
    return new Promise((resolve, reject) => {
        try {
            db_1.mysqlPoolConnection.query('call up_GetChatMessageById(?)', [
                ChatHistoryId !== null && ChatHistoryId !== void 0 ? ChatHistoryId : 0,
            ], (err, result) => {
                if (err) {
                    reject(err);
                }
                resolve(result);
            });
        }
        catch (error) {
            reject(error);
        }
    });
};
exports.getCustomerChatHistory = getCustomerChatHistory;
const createCustomerCallBack = (data) => {
    return new Promise((resolve, reject) => {
        const createdAt = Date.now();
        try {
            // const mysqlq = `INSERT INTO callBack (sessionId,customerData, domainId,insertAt)
            // VALUES ('${data.session_id}',${data.customerData}, ${data.domain_id},${createdAt}) `;
            // mysqlPoolConnection.query(mysqlq, (err: any, result: any) => {
            //   if (err) {
            //     reject(err);
            //   } else {
            //     resolve(result);
            //   }
            // });
        }
        catch (error) {
            reject(error);
        }
    });
};
exports.createCustomerCallBack = createCustomerCallBack;
/**
 *
 * @param data
 * @returns
 */
const createCustomerFilepath = (data) => {
    return new Promise((resolve, reject) => {
        try {
            const mysqlq = `UPDATE  sessionHistory SET customerFile = '${data.url}' where sessionId = '${data.sessionId}'`;
            db_1.mysqlPoolConnection.query(mysqlq, (err, result) => {
                if (err) {
                    reject(err);
                }
                else {
                    resolve(result);
                }
            });
        }
        catch (error) {
            reject(error);
        }
    });
};
exports.createCustomerFilepath = createCustomerFilepath;
/**
 *
 * @param data
 * @returns
 */
const roleInfoDao = (data) => {
    return new Promise((resolve, reject) => {
        try {
            const mysqlq = `SELECT UserID, roleid from tb_User where domainId=${data.domainId} AND ext=${data.ext}`;
            db_1.mysqlPoolConnection.query(mysqlq, (err, result) => {
                if (err) {
                    reject(err);
                }
                else {
                    resolve(result[0]);
                }
            });
        }
        catch (error) {
            reject(error);
        }
    });
};
exports.roleInfoDao = roleInfoDao;
const updateMessageDao = (data, messageArr) => {
    return new Promise((resolve, reject) => __awaiter(void 0, void 0, void 0, function* () {
        try {
            db_1.mysqlPoolConnection.query('call up_SetChatMessage(?,?,?)', [data === null || data === void 0 ? void 0 : data.ChatDurationHistoryId, data === null || data === void 0 ? void 0 : data.ChatHistoryId, JSON.stringify(messageArr)], (err, result) => {
                if (err) {
                    reject(err);
                    console.log(err);
                }
                resolve(result);
            });
        }
        catch (err) {
            reject(err);
        }
    }));
};
exports.updateMessageDao = updateMessageDao;
const getSessionDetailsDao = (data) => {
    return new Promise((resolve, reject) => {
        try {
            const mysqlq = `SELECT * FROM sessionHistory WHERE chatFlowSourceId = '${data.chatFlowId}'`;
            db_1.mysqlPoolConnection.query(mysqlq, (err, result) => {
                if (err) {
                    reject(err);
                }
                else {
                    resolve(result);
                }
            });
        }
        catch (error) {
            reject(error);
        }
    });
};
exports.getSessionDetailsDao = getSessionDetailsDao;
const getRoutingProfile = (qid) => {
    return new Promise((resolve, reject) => {
        try {
            const mysqlqs = `SELECT * FROM routingqueueprofilemapping WHERE qid = ${qid}`;
            db_1.mysqlPoolConnection.query(mysqlqs, (err, results) => {
                var _a;
                if (err) {
                    reject(err);
                }
                else {
                    const resultData = (_a = results === null || results === void 0 ? void 0 : results.map((elements) => elements === null || elements === void 0 ? void 0 : elements.rqmid)) !== null && _a !== void 0 ? _a : [];
                    resolve(resultData);
                }
            });
        }
        catch (error) {
            reject(error);
        }
    });
};
exports.getRoutingProfile = getRoutingProfile;
const getRoutingProfiles = (queueId, domainId) => {
    return new Promise((resolve, reject) => {
        try {
            db_1.mysqlPoolConnection.query('call ccaas_get_profile_mapping_priority_by_qid(?,?)', [domainId, queueId], (err, res) => {
                if (err) {
                    reject(err);
                }
                else {
                    resolve(res[0]);
                }
            });
        }
        catch (error) {
            reject(error);
        }
    });
};
exports.getRoutingProfiles = getRoutingProfiles;
const getRoutingDetails = (rqmid, domainId) => {
    return new Promise((resolve, reject) => __awaiter(void 0, void 0, void 0, function* () {
        try {
            const routingp = `select * from routing_profile WHERE rid = ${rqmid} AND domainId = ${domainId}`;
            db_1.mysqlPoolConnection.query(routingp, (err, result) => {
                if (err) {
                    reject(err);
                }
                else {
                    resolve(result[0]);
                }
            });
        }
        catch (error) {
            reject(error);
        }
    }));
};
exports.getRoutingDetails = getRoutingDetails;
const getPreviousStatus = (chatId, domainId) => {
    return new Promise((resolve, reject) => __awaiter(void 0, void 0, void 0, function* () {
        try {
            const sp = 'call ccaas_get_user_chat_tracking_log(?,?)';
            db_1.mysqlPoolConnection.query(sp, [domainId, chatId], (err, result) => {
                if (err) {
                    reject(err);
                }
                else {
                    resolve(JSON.parse(JSON.stringify(result[0])));
                }
            });
        }
        catch (error) {
            reject(error);
        }
    }));
};
exports.getPreviousStatus = getPreviousStatus;
const insertStatus = (chatId, domainId, obj) => {
    return new Promise((resolve, reject) => __awaiter(void 0, void 0, void 0, function* () {
        try {
            const sp = 'call ccaas_insert_update_user_chat_tracking_log(?,?,?)';
            db_1.mysqlPoolConnection.query(sp, [domainId, chatId, JSON.stringify(obj)], (err, result) => {
                if (err) {
                    reject(err);
                }
                else {
                    resolve(JSON.parse(JSON.stringify(result[0])));
                }
            });
        }
        catch (error) {
            reject(error);
        }
    }));
};
exports.insertStatus = insertStatus;
const getStatusData = ({ ext, domainId, isVoice, isChat, isEmail, status }) => {
    return {
        domainId,
        ext,
        isVoice,
        isChat,
        isEmail,
        isVoiceCallTransfer: isVoice,
        isChatTransfer: isChat,
        isEmailTransfer: isEmail,
        stateTime: Math.floor(Date.now() / 1000),
        statusName: status,
    };
};
exports.getStatusData = getStatusData;
const getAgentListChat = (qid, channel, adaptor) => {
    console.log('adaptor >>>>>>', channel);
    return new Promise((resolve, reject) => {
        try {
            const mysqlqs = `SELECT * FROM routingqueueprofilemapping WHERE qid = ${qid}`;
            db_1.mysqlPoolConnection.query(mysqlqs, (err, results) => {
                var _a;
                if (err) {
                    console.log('ERROR: ', err);
                    reject(err);
                }
                else {
                    const ids = results === null || results === void 0 ? void 0 : results.map((data) => data.rqmid);
                    db_1.mysqlPoolConnection.query('call up_GetAgentListChat(?,?,?)', [
                        channel === null || channel === void 0 ? void 0 : channel.domain_id,
                        ids === null || ids === void 0 ? void 0 : ids.toString(),
                        (_a = channel === null || channel === void 0 ? void 0 : channel.ext) !== null && _a !== void 0 ? _a : null
                    ], (errors, res) => {
                        var _a;
                        if (errors) {
                            console.log(errors);
                            reject([]);
                        }
                        else {
                            if (((_a = res[0]) === null || _a === void 0 ? void 0 : _a.length) > 0) {
                                // mysqlPoolConnection.query('call ccaas_StatusIdStateTimer(?,?,?,?)', [
                                //   res[0][0]?.ext,
                                //   res[0][0]?.domainId,
                                //   'Busy',
                                //   `${Math.floor(Date.now() / 1000)}`
                                // ],
                                //   (_: any, __: any) => {
                                //     const agentDetails = res[0];
                                //     const data = getStatusData({ ext: agentDetails?.ext, domainId: agentDetails?.domainId, isVoice: 0, isChat: 0, isEmail: 0, status: 'Busy' });
                                // adaptor.emitMessage(`${agentDetails?.domainId}_${agentDetails?.ext}`, 'userStatusBackendTransfer', data);
                                resolve(res);
                                // });
                            }
                            else {
                                resolve([]);
                            }
                        }
                    });
                    // mysqlPoolConnection.query('call up_GetAgentListChat(?,?)', (errors, res) => {
                    //   if (errors) {
                    //     console.log(errors);
                    //     reject([]);
                    //   } else {
                    //     if (res?.length > 0) {
                    //       const updateQuery = `UPDATE tb_User SET statusName = 'Busy', stateTimer = '${Math.floor(Date.now() / 1000)}' where uid = ${res[0].uid}`;
                    //       mysqlPoolConnection.query(updateQuery, (_: any, __: any) => {
                    //         const agentDetails = res[0];
                    //         const data = getStatusData({ ext: agentDetails?.ext, domainId: agentDetails?.domainId, isVoice: 0, isChat: 0, isEmail: 0, status: 'Busy' });
                    //         // adaptor.emitMessage(`${agentDetails?.domainId}_${agentDetails?.ext}`, 'userStatusBackendTransfer', data);
                    //         resolve(res);
                    //       });
                    //     } else {
                    //       resolve([]);
                    //     }
                    //   }
                    // });
                }
            });
        }
        catch (error) {
            reject(error);
        }
    });
};
exports.getAgentListChat = getAgentListChat;
const getIncomingChatDetailsDB = (channelId, queueId) => {
    return new Promise((resolve, reject) => {
        try {
            db_1.mysqlPoolConnection.query(`SELECT * FROM queueWeighTimeChat WHERE domainId = ${(channelId === null || channelId === void 0 ? void 0 : channelId.domain_id) || (channelId === null || channelId === void 0 ? void 0 : channelId.domainId)} AND qid=${queueId}`, (errors, res) => {
                if (errors) {
                    console.log(errors);
                    reject([]);
                }
                else {
                    if ((res === null || res === void 0 ? void 0 : res.length) > 0) {
                        resolve(JSON.parse(JSON.stringify(res)));
                    }
                    else {
                        resolve([]);
                    }
                }
            });
        }
        catch (error) {
            reject(error);
        }
    });
};
exports.getIncomingChatDetailsDB = getIncomingChatDetailsDB;
const insertIncomingChatsDB = (channelId, queueId) => {
    return new Promise((resolve, reject) => {
        try {
            db_1.mysqlPoolConnection.query('call up_InsertUpdatequeueWeighTimeChat(?,?,?,?)', [
                queueId,
                (channelId === null || channelId === void 0 ? void 0 : channelId.call_type) || '',
                (channelId === null || channelId === void 0 ? void 0 : channelId.domain_id) || (channelId === null || channelId === void 0 ? void 0 : channelId.domainId),
                JSON.stringify(channelId === null || channelId === void 0 ? void 0 : channelId.queueList)
            ], (errors, res) => {
                var _a;
                if (errors) {
                    console.log(errors);
                    reject([]);
                }
                else {
                    if (((_a = res[0]) === null || _a === void 0 ? void 0 : _a.length) > 0) {
                        resolve((res[0]));
                    }
                    else {
                        resolve([]);
                    }
                }
            });
        }
        catch (error) {
            reject(error);
        }
    });
};
exports.insertIncomingChatsDB = insertIncomingChatsDB;
const getTransferAgentStatus = (qid, channel) => {
    return new Promise((resolve, reject) => {
        var _a;
        try {
            db_1.mysqlPoolConnection.query('call up_CheckUserReady(?,?)', [
                qid !== null && qid !== void 0 ? qid : null,
                (_a = channel === null || channel === void 0 ? void 0 : channel.domain_id) !== null && _a !== void 0 ? _a : 0,
            ], (errors, res) => {
                var _a;
                if (errors) {
                    console.log(errors);
                    reject([]);
                }
                else {
                    if (((_a = res[0]) === null || _a === void 0 ? void 0 : _a.length) > 0) {
                        resolve(res);
                    }
                    else {
                        resolve([]);
                    }
                }
            });
        }
        catch (error) {
            reject(error);
        }
    });
};
exports.getTransferAgentStatus = getTransferAgentStatus;
const getListChatAgent = (channel) => {
    console.log('adaptor >>>>>>', channel);
    return new Promise((resolve, reject) => {
        var _a;
        try {
            db_1.mysqlPoolConnection.query('call up_GetAgentListChat(?,?,?)', [
                channel === null || channel === void 0 ? void 0 : channel.domain_id,
                null,
                (_a = channel === null || channel === void 0 ? void 0 : channel.ext) !== null && _a !== void 0 ? _a : null
            ], (errors, res) => {
                var _a;
                if (errors) {
                    console.log(errors);
                    reject([]);
                }
                else {
                    if (((_a = res[0]) === null || _a === void 0 ? void 0 : _a.length) > 0) {
                        resolve(res);
                    }
                    else {
                        resolve([]);
                    }
                }
            });
        }
        catch (error) {
            reject(error);
        }
    });
};
exports.getListChatAgent = getListChatAgent;
const getJSON = (url, callback) => {
    axios_1.default.get(url)
        .then((response) => {
        callback(null, response.data);
    })
        .catch((error) => {
        callback(error.response.status, error.response.data);
    });
};
exports.getJSON = getJSON;
const getQueueSettings = (qid) => {
    return new Promise((resolve, reject) => {
        try {
            const mysqlqs = `SELECT * FROM queue WHERE qid = ${qid} `;
            db_1.mysqlPoolConnection.query(mysqlqs, (err, results) => {
                if (err) {
                    reject(err);
                }
                else {
                    resolve(results[0]);
                }
            });
        }
        catch (error) {
            console.log('error', error);
            reject(error);
        }
    });
};
exports.getQueueSettings = getQueueSettings;
/**
 *
 * @param queueData
 * @returns
 */
const getHoursofOperation = (queueData) => {
    let data = '';
    return new Promise((resolve, reject) => {
        try {
            const mysqlq = `SELECT * FROM queue WHERE qid = ${queueData !== null && queueData !== void 0 ? queueData : null} `;
            db_1.mysqlPoolConnection.query(mysqlq, (err, results) => {
                var _a;
                if (err) {
                    console.log(err);
                    reject(err);
                }
                else {
                    console.log({ results, mysqlq });
                    data = results[0];
                    const mysqlqs = `SELECT * FROM custom_hours WHERE cid = ${(_a = data === null || data === void 0 ? void 0 : data.hoursOfOperatioId) !== null && _a !== void 0 ? _a : null} `;
                    db_1.mysqlPoolConnection.query(mysqlqs, (error, hoursResult) => {
                        var _a, _b, _c, _d, _e, _f, _g, _h, _j, _k;
                        if (error) {
                            console.log('error', error);
                        }
                        else {
                            try {
                                const hours = JSON.parse((_b = (_a = hoursResult[0]) === null || _a === void 0 ? void 0 : _a.hours) !== null && _b !== void 0 ? _b : 'null');
                                // let timeZone:any = hoursResult[0]?.timeZone.split(" ")[0];
                                const timeZone = (_d = (_c = hoursResult[0]) === null || _c === void 0 ? void 0 : _c.timeZoneValue) !== null && _d !== void 0 ? _d : 'Europe/London';
                                const Currentday = (_f = (_e = (0, moment_timezone_1.default)()) === null || _e === void 0 ? void 0 : _e.tz(timeZone)) === null || _f === void 0 ? void 0 : _f.format('dddd');
                                const CurrentDayHours = hours === null || hours === void 0 ? void 0 : hours.filter((item) => item.day === Currentday);
                                const currentTime = (_h = (_g = (0, moment_timezone_1.default)()) === null || _g === void 0 ? void 0 : _g.tz(timeZone)) === null || _h === void 0 ? void 0 : _h.valueOf();
                                const timeArray = [];
                                if (CurrentDayHours && (0, lodash_1.isArray)(CurrentDayHours)) {
                                    for (const i of (_k = (_j = CurrentDayHours[0]) === null || _j === void 0 ? void 0 : _j.entry) !== null && _k !== void 0 ? _k : []) {
                                        const startTime = (0, moment_timezone_1.default)(Number(i === null || i === void 0 ? void 0 : i.startTimeStamp)).valueOf() - 3600000;
                                        const endTime = (0, moment_timezone_1.default)(Number(i === null || i === void 0 ? void 0 : i.endTimeStamp)).valueOf() - 3600000;
                                        timeArray.push([startTime, endTime]);
                                    }
                                    for (const time of timeArray !== null && timeArray !== void 0 ? timeArray : []) {
                                        console.log(time[0], time[1], currentTime);
                                        if (currentTime >= time[0] && currentTime <= time[1]) {
                                            resolve(true);
                                        }
                                    }
                                }
                                resolve(false);
                            }
                            catch (error) {
                                console.log(error);
                                reject(false);
                            }
                        }
                    });
                }
            });
        }
        catch (error) {
            console.log(error);
            reject(error);
        }
    });
};
exports.getHoursofOperation = getHoursofOperation;
const findAgentByEmail = (qid, domainId, email, adaptor) => {
    return new Promise((resolve, reject) => {
        try {
            const query = `SELECT * FROM tb_User WHERE domainId = ${domainId} AND emailId = '${email}'`;
            db_1.mysqlPoolConnection.query(query, (errors, res) => {
                if (errors) {
                    reject([]);
                }
                else {
                    if ((res === null || res === void 0 ? void 0 : res.length) > 0) {
                        const updateQuery = `UPDATE tb_User SET statusName = 'Busy', stateTimer = '${(0, moment_timezone_1.default)().format('x')}' where uid = ${res[0].uid}`;
                        db_1.mysqlPoolConnection.query(updateQuery, (_, __) => {
                            if ((res === null || res === void 0 ? void 0 : res.length) > 0) {
                                const agentDetails = res[0];
                                const data = (0, exports.getStatusData)({ ext: agentDetails === null || agentDetails === void 0 ? void 0 : agentDetails.ext, domainId: agentDetails === null || agentDetails === void 0 ? void 0 : agentDetails.domainId, isVoice: 0, isChat: 0, isEmail: 0, status: 'Busy' });
                                // adaptor.emitMessage(`${agentDetails?.domainId}_${agentDetails?.ext}`, 'userStatusBackendTransfer', data);
                            }
                            resolve(res);
                        });
                        resolve(res);
                    }
                    else {
                        resolve([]);
                    }
                }
            });
        }
        catch (error) {
            console.log('error', error);
            reject(error);
        }
    });
};
exports.findAgentByEmail = findAgentByEmail;
const getDomainStatus = (data) => __awaiter(void 0, void 0, void 0, function* () {
    return new Promise((resolve, reject) => __awaiter(void 0, void 0, void 0, function* () {
        try {
            db_1.mysqlPoolConnection.query('call ccaas_GetUserAvaiableStatus(?)', [
                data,
            ], (errors, res) => {
                var _a;
                if (errors) {
                    console.log(errors);
                    reject([]);
                }
                else {
                    if (((_a = res[0]) === null || _a === void 0 ? void 0 : _a.length) > 0) {
                        resolve(res[0]);
                    }
                    else {
                        resolve([]);
                    }
                }
            });
        }
        catch (error) {
            console.log(error);
        }
    }));
});
exports.getDomainStatus = getDomainStatus;
const getChatHoursOfOperation = (data) => __awaiter(void 0, void 0, void 0, function* () {
    return new Promise((resolve, reject) => __awaiter(void 0, void 0, void 0, function* () {
        try {
            const mysqlq = 'SELECT * FROM custom_hours WHERE cid=1356';
            // const mysqlq = `SELECT * FROM custom_hours WHERE cid=${data?.cid}`;
            db_1.mysqlPoolConnection.query(mysqlq, (err, results) => {
                var _a, _b, _c, _d, _e, _f, _g, _h, _j, _k, _l;
                if (err) {
                    console.log(err);
                    reject(err);
                }
                else {
                    try {
                        const hours = JSON.parse((_b = (_a = results[0]) === null || _a === void 0 ? void 0 : _a.hours) !== null && _b !== void 0 ? _b : 'null');
                        const timeZone = (_c = results[0]) === null || _c === void 0 ? void 0 : _c.timeZoneValue;
                        const Currentday = (_e = (_d = (0, moment_timezone_1.default)()) === null || _d === void 0 ? void 0 : _d.tz(timeZone)) === null || _e === void 0 ? void 0 : _e.format('dddd');
                        const CurrentDayHours = hours === null || hours === void 0 ? void 0 : hours.filter((item) => {
                            if (item.day === Currentday) {
                                return item;
                            }
                        });
                        const currentTime = (_g = (_f = (0, moment_timezone_1.default)(new Date())) === null || _f === void 0 ? void 0 : _f.tz(timeZone)) === null || _g === void 0 ? void 0 : _g.format('HH:mm');
                        const timeArray = [];
                        for (const i of (_j = (_h = CurrentDayHours[0]) === null || _h === void 0 ? void 0 : _h.entry) !== null && _j !== void 0 ? _j : []) {
                            const startTime = (0, moment_timezone_1.default)(new Date(parseInt(i === null || i === void 0 ? void 0 : i.startTimeStamp, 10))).format('HH:mm');
                            const endTime = (0, moment_timezone_1.default)(new Date(parseInt(i === null || i === void 0 ? void 0 : i.endTimeStamp, 10))).format('HH:mm');
                            timeArray.push([startTime, endTime]);
                        }
                        console.log('timeArray---', timeArray, currentTime, Currentday);
                        for (const time of timeArray !== null && timeArray !== void 0 ? timeArray : []) {
                            const [fromHour, fromMinute] = (_k = time[0]) === null || _k === void 0 ? void 0 : _k.split(':');
                            const [toHour, toMinute] = (_l = time[1]) === null || _l === void 0 ? void 0 : _l.split(':');
                            const [nowHour, nowMinute] = currentTime === null || currentTime === void 0 ? void 0 : currentTime.split(':');
                            const fromTimestamp = parseInt(fromHour, 10) * 3600000 + parseInt(fromMinute, 10) * 60000;
                            const toTimestamp = parseInt(toHour, 10) * 3600000 + parseInt(toMinute, 10) * 60000;
                            const nowTimestamp = parseInt(nowHour, 10) * 3600000 + parseInt(nowMinute, 10) * 60000;
                            if (nowTimestamp >= fromTimestamp && nowTimestamp <= toTimestamp) {
                                resolve(true);
                            }
                            else {
                                resolve(false);
                            }
                        }
                        resolve(false);
                    }
                    catch (error) {
                        console.log(error);
                    }
                }
            });
        }
        catch (error) {
            console.log(error);
        }
    }));
});
exports.getChatHoursOfOperation = getChatHoursOfOperation;
const getConcurrentChat = (data) => __awaiter(void 0, void 0, void 0, function* () {
    return new Promise((resolve, reject) => __awaiter(void 0, void 0, void 0, function* () {
        try {
            db_1.mysqlPoolConnection.query('call up_GetConcurrentAgentLimit(?)', [
                data
            ], (errors, res) => {
                var _a;
                if (errors) {
                    console.log(errors);
                    reject([]);
                }
                else {
                    if (((_a = res[0]) === null || _a === void 0 ? void 0 : _a.length) > 0) {
                        resolve(res[0][0]);
                    }
                    else {
                        resolve('null');
                    }
                }
            });
        }
        catch (error) {
            console.log(error);
        }
    }));
});
exports.getConcurrentChat = getConcurrentChat;
const getConcurrentChatLimit = (data) => __awaiter(void 0, void 0, void 0, function* () {
    return new Promise((resolve, reject) => __awaiter(void 0, void 0, void 0, function* () {
        try {
            db_1.mysqlPoolConnection.query('call ccaas_get_check_concurrrent_chat_limit(?,?)', [
                (data === null || data === void 0 ? void 0 : data.domainId) || 0,
                (data === null || data === void 0 ? void 0 : data.UserID) || 0
            ], (errors, res) => {
                var _a;
                if (errors) {
                    console.log(errors);
                    reject([]);
                }
                else {
                    if (((_a = res[0]) === null || _a === void 0 ? void 0 : _a.length) > 0) {
                        resolve(res[0][0]);
                    }
                    else {
                        resolve('null');
                    }
                }
            });
        }
        catch (error) {
            console.log(error);
        }
    }));
});
exports.getConcurrentChatLimit = getConcurrentChatLimit;
/**
 *
 * @param domainId
 * @returns
 */
const getRoutingProfileUser = (domainId) => {
    return new Promise((resolve, reject) => {
        try {
            db_1.mysqlPoolConnection.query('call up_GetUserConcurrentChat(?)', [
                domainId
            ], (errors, res) => {
                var _a;
                if (errors) {
                    console.log(errors);
                    reject([]);
                }
                else {
                    if (((_a = res[0]) === null || _a === void 0 ? void 0 : _a.length) > 0) {
                        resolve(res[0]);
                    }
                    else {
                        resolve([]);
                    }
                }
            });
        }
        catch (error) {
            reject(error);
        }
    });
};
exports.getRoutingProfileUser = getRoutingProfileUser;
/**
 *
 * @param data
 * @param dataq
 * @returns
 */
// export const getAgentByChat = (domainId: any) => {
//   return new Promise((resolve, reject) => {
//     try {
//       mysqlPoolConnection.query(
//         'call ccaas_getUserListByRoleHandler(?,?)',
//         [
//           domainId,
//           '3,5,6,'
//         ],
//         (err: any, result: any) => {
//           if (err) {
//             reject(err);
//             console.log(err);
//           } else {
//             if (result[0]?.length > 0) {
//               resolve(result[0][0]);
//             } else {
//               resolve('null');
//             }
//           }
//         }
//       );
//     } catch (error) {
//       reject(error);
//     }
//   });
// };
const getAgentsSkills = (domainId, roleId) => {
    return new Promise((resolve, reject) => {
        try {
            db_1.mysqlPoolConnection.query('call ccaas_getUserListByRoleHandler(?,?)', [domainId, roleId], (err, result) => {
                if (err) {
                    reject(err);
                    console.log('getUserListByRole reject err: ', err);
                }
                else {
                    resolve(JSON.parse(JSON.stringify(result[0])));
                }
            });
        }
        catch (error) {
            reject(error);
        }
    });
};
exports.getAgentsSkills = getAgentsSkills;
const getAgentByChat = (domainId) => {
    return new Promise((resolve, reject) => {
        try {
            const mysqlq = `SELECT * FROM tb_User WHERE domainId = '${domainId}' AND chatLimit <> 0`;
            db_1.mysqlPoolConnection.query(mysqlq, (err, result) => {
                if (err) {
                    reject(err);
                }
                else {
                    resolve(result);
                }
            });
        }
        catch (error) {
            reject(error);
        }
    });
};
exports.getAgentByChat = getAgentByChat;
const getAgentByChatZero = (domainId) => {
    return new Promise((resolve, reject) => {
        try {
            const mysqlq = `SELECT * FROM tb_User WHERE domainId = '${domainId}'`;
            db_1.mysqlPoolConnection.query(mysqlq, (err, result) => {
                if (err) {
                    reject(err);
                }
                else {
                    resolve(result);
                }
            });
        }
        catch (error) {
            reject(error);
        }
    });
};
exports.getAgentByChatZero = getAgentByChatZero;
const insertApiCallCount = (data) => {
    return new Promise((resolve, reject) => {
        try {
            db_1.mysqlPoolConnection.query('call up_insert_API_Calls(?,?,?)', [data === null || data === void 0 ? void 0 : data.domainId, data === null || data === void 0 ? void 0 : data.chatFlowId, data === null || data === void 0 ? void 0 : data.apiCallCount]);
        }
        catch (error) {
            reject(error);
        }
    });
};
exports.insertApiCallCount = insertApiCallCount;
//# sourceMappingURL=callflow.dao.js.map