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
exports.updateDispositionForSms = exports.updateCalledPersonDetailsCall = exports.updateDispositionForCall = exports.isValidDispositionName = exports.updateDisposition = exports.updateDispositionStatus = exports.getDispositinStatus = exports.getAgentListByDomianId = exports.insertDispositionStatus = void 0;
const moment_1 = __importDefault(require("moment"));
const dbRetryUtil_1 = require("../helpers/dbRetryUtil");
const db_1 = require("../plugins/db");
/**
 *
 * @param data
 * @returns
 */
const insertDispositionStatus = (data) => {
    return new Promise((resolve, reject) => {
        try {
            const currentDate = (0, moment_1.default)(new Date()).format('YYYY-MM-DD HH:mm:ss');
            const disposition = `INSERT INTO disposition_status (dispositionName,type,description,status,domainId,createdAt, colorCode) VALUES ('${data.dispositionName}','${data.type}','${data.description}',${data.status},${data.domainId},'${currentDate}', '${data.colorCode}')`;
            db_1.mysqlPoolConnection.query(disposition, (err, result) => {
                if (err) {
                    reject(err);
                }
                else {
                    resolve({
                        did: result.insertId,
                        dispositionName: data.dispositionName,
                        type: data.type,
                        description: data.description,
                        status: data.status,
                        domainId: data.domainId,
                        colorCode: data.colorCode,
                        createdAt: currentDate,
                        updatedAt: '',
                    });
                }
            });
        }
        catch (error) {
            reject(error);
        }
    });
};
exports.insertDispositionStatus = insertDispositionStatus;
const getAgentListByDomianId = (data) => __awaiter(void 0, void 0, void 0, function* () {
    return new Promise((resolve, reject) => {
        try {
            db_1.mysqlPoolConnection.query('call ccaas_getDispositinStatus_info(?)', [
                data === null || data === void 0 ? void 0 : data.domainId
            ], (err, result) => {
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
});
exports.getAgentListByDomianId = getAgentListByDomianId;
/**
 *
 * @param data
 * @returns
 */
const getDispositinStatus = (data) => {
    return new Promise((resolve, reject) => {
        try {
            db_1.mysqlPoolConnection.query('call ccaas_getDispositinStatus_info(?)', [
                data === null || data === void 0 ? void 0 : data.domainId
            ], (err, result) => {
                if (err) {
                    console.log('ccaas_getDispositinStatus_info err: ', err);
                    reject(err);
                }
                resolve(result);
            });
        }
        catch (error) {
            console.log('ccaas_getDispositinStatus_info catch error: ', error);
            reject(error);
        }
    });
};
exports.getDispositinStatus = getDispositinStatus;
/**
 *
 * @param data
 * @param qdata
 * @returns
 */
const updateDispositionStatus = (data, did) => {
    return new Promise((resolve, reject) => {
        try {
            const currentDate = (0, moment_1.default)(new Date()).format('YYYY-MM-DD HH:mm:ss');
            const updateq = `UPDATE disposition_status SET dispositionName ='${data.dispositionName}',type = '${data.type}',description = '${data.description}',status = ${data.status},updatedAt = '${currentDate}', colorCode = '${data.colorCode}'  WHERE did =${did} AND domainId = ${data.domainId}`;
            db_1.mysqlPoolConnection.query(updateq, (err, result) => {
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
exports.updateDispositionStatus = updateDispositionStatus;
/**
 *
 * @param data
 * @returns
 */
const updateDisposition = (data) => {
    return new Promise((resolve, reject) => {
        const query = `UPDATE sessionHistory SET dispositionName = '${data.dispositionName}' WHERE domainId = ${data.domainId}`;
        db_1.mysqlPoolConnection.query(query, (error, results) => {
            if (error) {
                reject(error);
            }
            else {
                resolve(results);
            }
        });
    });
};
exports.updateDisposition = updateDisposition;
/**
 *
 * @param data
 * @returns
 */
const isValidDispositionName = (data) => {
    return new Promise((resolve, reject) => {
        try {
            const mysqlq = `SELECT * FROM disposition_status WHERE dispositionName= '${data.dispositionName}' AND domainId = ${data.domainId}`;
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
exports.isValidDispositionName = isValidDispositionName;
/**
 *
 * @param data
 * @returns
 */
const updateDispositionForCall = (data) => __awaiter(void 0, void 0, void 0, function* () {
    try {
        if (((data === null || data === void 0 ? void 0 : data.channelType) === 'Email' || (data === null || data === void 0 ? void 0 : data.channelType) === 'Sms') && ((data === null || data === void 0 ? void 0 : data.dispositionName) === 'Closed' || (data === null || data === void 0 ? void 0 : data.dispositionName) === 'Resolved')) {
            data.markAsRead = 1;
        }
        else {
            data.markAsRead = 0;
        }
        console.log('ccaas_UpdateSessionDisposition_details', [
            data.domainId,
            data.sessionId,
            data.summary,
            data.disposition.did,
            data.afterCallWorkTime,
            data.calledPersonDetails.coid,
            data.subject,
            data === null || data === void 0 ? void 0 : data.markAsRead
        ]);
        const result = yield (0, dbRetryUtil_1.executeMySQLWithRetry)(db_1.mysqlPoolConnection, 'call ccaas_UpdateSessionDisposition_details(?,?,?,?,?,?,?,?)', [
            data.domainId,
            data.sessionId,
            data.summary,
            data.disposition.did,
            data.afterCallWorkTime,
            data.calledPersonDetails.coid,
            data.subject,
            data === null || data === void 0 ? void 0 : data.markAsRead
        ], {
            operationName: 'UpdateCallDisposition_Socket',
            logContext: { sessionId: data === null || data === void 0 ? void 0 : data.sessionId, domainId: data === null || data === void 0 ? void 0 : data.domainId }
        });
        return (result === null || result === void 0 ? void 0 : result.length) > 0 ? result[0] : null;
    }
    catch (err) {
        console.log('updateDispositionForCall err:', err);
        throw err;
    }
});
exports.updateDispositionForCall = updateDispositionForCall;
/**
 *
 * @param data
 * @returns
 */
const updateCalledPersonDetailsCall = (data) => {
    return new Promise((resolve, reject) => {
        var _a;
        db_1.mysqlPoolConnection.query('CALL ccaas_UpdateCalledPersonDetails(?,?,?)', [(_a = data === null || data === void 0 ? void 0 : data.calledPersonDetails) === null || _a === void 0 ? void 0 : _a.coid, data === null || data === void 0 ? void 0 : data.domainId, data === null || data === void 0 ? void 0 : data.sessionId], (err, result) => {
            if (err) {
                reject(err);
                console.log(err);
            }
            resolve(result);
        });
    });
};
exports.updateCalledPersonDetailsCall = updateCalledPersonDetailsCall;
/**
 *
 * @param data
 * @returns
 */
const updateDispositionForSms = (data) => __awaiter(void 0, void 0, void 0, function* () {
    var _a, _b, _c, _d;
    try {
        const result = yield (0, dbRetryUtil_1.executeMySQLWithRetry)(db_1.mysqlPoolConnection, 'call up_UpdateSMSHistoryDisposition(?,?,?,?,?,?)', [
            data === null || data === void 0 ? void 0 : data.SMSHistoryId,
            data === null || data === void 0 ? void 0 : data.domainId,
            (_a = data === null || data === void 0 ? void 0 : data.dispositionId) !== null && _a !== void 0 ? _a : null,
            (_b = data === null || data === void 0 ? void 0 : data.summary) !== null && _b !== void 0 ? _b : null,
            (_c = data === null || data === void 0 ? void 0 : data.isSessionClosed) !== null && _c !== void 0 ? _c : 0,
            (_d = data === null || data === void 0 ? void 0 : data.AfterSMSWorkTime) !== null && _d !== void 0 ? _d : null
        ], {
            operationName: 'UpdateSMSDisposition',
            logContext: { SMSHistoryId: data === null || data === void 0 ? void 0 : data.SMSHistoryId, domainId: data === null || data === void 0 ? void 0 : data.domainId }
        });
        return result === null || result === void 0 ? void 0 : result[0];
    }
    catch (err) {
        console.log('updateDispositionForSms err:', err);
        throw err;
    }
});
exports.updateDispositionForSms = updateDispositionForSms;
//# sourceMappingURL=dispositionStatus.js.map