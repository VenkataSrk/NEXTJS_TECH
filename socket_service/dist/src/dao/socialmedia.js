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
exports.getDeclineBot = exports.getCompanyDetails = exports.configurationInstaDetails = exports.configurationFbDetails = exports.configurationEmailDetails = exports.configurationDetails = exports.getKnowledgeBaseId = exports.AgentAssistEmailSms = exports.AgentAssistSocialMedia = exports.updateMissedChatInsta = exports.sendMessageInstagram = exports.updateCallbackStatusInsta = exports.sendCallbackRequestInsta = exports.getCustomerCoIdInsta = exports.getInstagramConfigData = exports.updateQueueIdInsta = exports.updateAgentDetailsInsta = exports.updateIsBotInsta = exports.updateChatMessageInsta = void 0;
const axios_1 = __importDefault(require("axios"));
const db_1 = require("../plugins/db");
const log_1 = require("../plugins/log");
/**
 *
 * @param data
 * @returns
 */
const updateChatMessageInsta = (data) => {
    const message = JSON.stringify(data.message);
    const chat_id = parseInt(data === null || data === void 0 ? void 0 : data.historyId, 10);
    return new Promise((resolve, reject) => {
        try {
            db_1.mysqlPoolConnection.query('call up_InserUpdateInstagramMessage(?,?)', [chat_id !== null && chat_id !== void 0 ? chat_id : 0, message], (err, result) => {
                if (err) {
                    reject(err);
                }
                else {
                    if (result && (result === null || result === void 0 ? void 0 : result.length) > 0) {
                        resolve(result);
                    }
                    else {
                        reject(err);
                    }
                }
            });
        }
        catch (error) {
            console.log(error);
        }
    });
};
exports.updateChatMessageInsta = updateChatMessageInsta;
/**
 *
 * @param HistoryId
 * @returns
 */
const updateIsBotInsta = (HistoryId) => {
    return new Promise((resolve, reject) => __awaiter(void 0, void 0, void 0, function* () {
        try {
            db_1.mysqlPoolConnection.query('CALL ccaas_update_flow_bot_check_insta(?)', [
                HistoryId
            ], (err, result) => {
                if (err) {
                    console.log(err);
                    reject(err);
                }
                if (result && result.length > 0) {
                    resolve(result[0]);
                }
                else {
                    resolve([]);
                }
            });
        }
        catch (err) {
            console.log(err);
            reject(err);
        }
    }));
};
exports.updateIsBotInsta = updateIsBotInsta;
/**
 *
 * @param userId
 * @returns
 */
const updateAgentDetailsInsta = (data) => {
    return new Promise((resolve, reject) => {
        var _a, _b, _c, _d, _e, _f;
        try {
            db_1.mysqlPoolConnection.query('call up_UpdateInstagramHistory(?,?,?,?,?,?,?,?,?)', [
                data === null || data === void 0 ? void 0 : data.InstagramHistoryId,
                (_a = data === null || data === void 0 ? void 0 : data.AgentId) !== null && _a !== void 0 ? _a : null,
                data === null || data === void 0 ? void 0 : data.domainId,
                data === null || data === void 0 ? void 0 : data.IsBot,
                (_b = data === null || data === void 0 ? void 0 : data.BotId) !== null && _b !== void 0 ? _b : null,
                (_c = data === null || data === void 0 ? void 0 : data.agentStartTime) !== null && _c !== void 0 ? _c : null,
                (_d = data === null || data === void 0 ? void 0 : data.agentEndTime) !== null && _d !== void 0 ? _d : null,
                (_e = data === null || data === void 0 ? void 0 : data.duration) !== null && _e !== void 0 ? _e : null,
                (_f = data === null || data === void 0 ? void 0 : data.dialDuration) !== null && _f !== void 0 ? _f : null,
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
exports.updateAgentDetailsInsta = updateAgentDetailsInsta;
/**
 *
 * @param data
 * @returns
 */
const updateQueueIdInsta = (data, queueId) => {
    return new Promise((resolve, reject) => __awaiter(void 0, void 0, void 0, function* () {
        try {
            db_1.mysqlPoolConnection.query('CALL up_UpdateInstagramQueueidBasedOnDomainId(?,?)', [
                data === null || data === void 0 ? void 0 : data.InstagramHistoryId,
                queueId !== null && queueId !== void 0 ? queueId : null
            ], (err, result) => {
                if (err) {
                    console.log(err);
                    reject(err);
                }
                if (result && result.length > 0) {
                    resolve(result[0]);
                }
                else {
                    resolve([]);
                }
            });
        }
        catch (err) {
            reject(err);
        }
    }));
};
exports.updateQueueIdInsta = updateQueueIdInsta;
/**
 *
 * @param data
 * @returns
 */
const getInstagramConfigData = (data) => {
    return new Promise((resolve, reject) => __awaiter(void 0, void 0, void 0, function* () {
        var _a, _b;
        try {
            db_1.mysqlPoolConnection.query('call up_CheckInstagramBot (?,?)', [
                (_a = data === null || data === void 0 ? void 0 : data.agentNumber) !== null && _a !== void 0 ? _a : null,
                (_b = data === null || data === void 0 ? void 0 : data.domainId) !== null && _b !== void 0 ? _b : null,
            ], (err, result) => {
                if (err) {
                    reject(err);
                    console.log(err);
                }
                if (result && result.length > 0) {
                    resolve(result[0]);
                }
                else {
                    reject(err);
                }
            });
        }
        catch (err) {
            reject(err);
        }
    }));
};
exports.getInstagramConfigData = getInstagramConfigData;
/**
 *
 * @param data
 * @returns
 */
const getCustomerCoIdInsta = (data) => {
    return new Promise((resolve, reject) => __awaiter(void 0, void 0, void 0, function* () {
        try {
            db_1.mysqlPoolConnection.query('call ccaas_get_coid_by_customerId(?,?,?)', [
                data === null || data === void 0 ? void 0 : data.customerNumber,
                data === null || data === void 0 ? void 0 : data.domainId,
                'Instagram'
            ], (err, result) => {
                if (err) {
                    console.log(err);
                    reject(err);
                }
                if (result && result.length > 0) {
                    resolve(result[0]);
                }
                else {
                    resolve([]);
                }
            });
        }
        catch (err) {
            reject(err);
        }
    }));
};
exports.getCustomerCoIdInsta = getCustomerCoIdInsta;
// send Button fb
function sendCallbackRequestInsta(req, buttonLink) {
    return __awaiter(this, void 0, void 0, function* () {
        try {
            const body = {
                domainId: req === null || req === void 0 ? void 0 : req.domainId,
                customerId: req === null || req === void 0 ? void 0 : req.customerNumber,
                pageId: req === null || req === void 0 ? void 0 : req.agentNumber,
                postbackText: 'Our agents are currently not available. Please feel free to leave a message with a brief summary of your inquiry along with your contact information, or you can get in touch with us at a later time.',
                buttons: [
                    {
                        type: 'web_url',
                        url: `${process.env.API_URL}/${buttonLink}`,
                        title: 'callback',
                        webview_height_ratio: 'full'
                    }
                ]
            };
            return yield axios_1.default.post(process.env.postbackButtonInsta, body);
        }
        catch (error) {
            console.error('Error sending message:', error);
        }
    });
}
exports.sendCallbackRequestInsta = sendCallbackRequestInsta;
/**
 *
 * @param domainId
 * @returns
 */
const updateCallbackStatusInsta = (data) => {
    return new Promise((resolve, reject) => {
        try {
            db_1.mysqlPoolConnection.query('call ccaas_update_instagram_callback_status(?,?)', [
                data === null || data === void 0 ? void 0 : data.domainId,
                data === null || data === void 0 ? void 0 : data.historyId
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
exports.updateCallbackStatusInsta = updateCallbackStatusInsta;
// send message facebook
function sendMessageInstagram(req) {
    return __awaiter(this, void 0, void 0, function* () {
        try {
            const body = {
                domainId: req === null || req === void 0 ? void 0 : req.domainId,
                customerId: req === null || req === void 0 ? void 0 : req.customerNumber,
                pageId: req === null || req === void 0 ? void 0 : req.agentNumber,
                message: 'Our agents are currently not available. Please feel free to leave a message with a brief summary of your inquiry along with your contact information, or you can get in touch with us at a later time.'
            };
            const response = yield axios_1.default.post(process.env.InstagramOut, body);
            return body;
        }
        catch (error) {
            console.error('Error sending message:', error);
            // You can handle the error here, e.g., log it or return a specific response
        }
    });
}
exports.sendMessageInstagram = sendMessageInstagram;
/**
 *
 * @param data
 * @returns
 */
const updateMissedChatInsta = (data) => {
    return new Promise((resolve, reject) => __awaiter(void 0, void 0, void 0, function* () {
        try {
            db_1.mysqlPoolConnection.query('call ccaas_update_socialMedia_missed_dtl(?,?,?)', [
                data === null || data === void 0 ? void 0 : data.domainId,
                data === null || data === void 0 ? void 0 : data.historyId,
                'Instagram'
            ], (err, result) => {
                if (err) {
                    console.log(err);
                    reject(err);
                }
                if (result && result.length > 0) {
                    resolve(result[0]);
                }
                else {
                    resolve([]);
                }
            });
        }
        catch (err) {
            console.log(err);
            reject(err);
        }
    }));
};
exports.updateMissedChatInsta = updateMissedChatInsta;
// Agent Assist SocialMedia
function AgentAssistSocialMedia(data, comDetails) {
    var _a, _b, _c, _d, _e, _f;
    return __awaiter(this, void 0, void 0, function* () {
        try {
            const message = '0';
            const body = {
                message,
                domainId: data === null || data === void 0 ? void 0 : data.domainId,
                ext: data === null || data === void 0 ? void 0 : data.agentExt,
                channelType: data === null || data === void 0 ? void 0 : data.channelType,
                channelId: (_b = (_a = data === null || data === void 0 ? void 0 : data.agentNumber) === null || _a === void 0 ? void 0 : _a.toString()) !== null && _b !== void 0 ? _b : '',
                sessionId: data === null || data === void 0 ? void 0 : data.sessionId,
                websiteId: '',
                type: '',
                company_name: (_c = comDetails === null || comDetails === void 0 ? void 0 : comDetails.companyName) !== null && _c !== void 0 ? _c : '',
                industry_type: (_d = comDetails === null || comDetails === void 0 ? void 0 : comDetails.industryType) !== null && _d !== void 0 ? _d : '',
                customerId: (_f = (_e = data === null || data === void 0 ? void 0 : data.customerNumber) === null || _e === void 0 ? void 0 : _e.toString()) !== null && _f !== void 0 ? _f : '',
            };
            console.log('Sending request to bot:', JSON.stringify(body, null, 2));
            const response = yield axios_1.default.post(process.env.semiChannel, body);
            console.log('Response from nlp>>>>>>>>>>>', JSON.stringify(response.data, null, 2));
            return response;
        }
        catch (error) {
            console.error('Error sending message:', error);
            // You can handle the error here, e.g., log it or return a specific response
        }
    });
}
exports.AgentAssistSocialMedia = AgentAssistSocialMedia;
// Agent Assist Email sms
function AgentAssistEmailSms(data, getWebsite) {
    var _a, _b, _c, _d, _e, _f, _g, _h, _j, _k, _l;
    return __awaiter(this, void 0, void 0, function* () {
        try {
            const message = JSON.parse(data === null || data === void 0 ? void 0 : data.message);
            const messagess = Array.isArray(message) ? (_a = message === null || message === void 0 ? void 0 : message.map((item) => { var _a, _b; return (_b = (_a = item === null || item === void 0 ? void 0 : item.userText) === null || _a === void 0 ? void 0 : _a.message) === null || _b === void 0 ? void 0 : _b.message; })) === null || _a === void 0 ? void 0 : _a.join(' , ') : (_c = (_b = message === null || message === void 0 ? void 0 : message.userText) === null || _b === void 0 ? void 0 : _b.message) === null || _c === void 0 ? void 0 : _c.message;
            const body = {
                message: messagess,
                domainId: data === null || data === void 0 ? void 0 : data.domainId,
                ext: data === null || data === void 0 ? void 0 : data.agentExt,
                channelType: data === null || data === void 0 ? void 0 : data.channelType,
                channelId: (_e = (_d = data === null || data === void 0 ? void 0 : data.agentNumber) === null || _d === void 0 ? void 0 : _d.toString()) !== null && _e !== void 0 ? _e : '',
                sessionId: data === null || data === void 0 ? void 0 : data.sessionId,
                websiteId: (_f = getWebsite.websiteId) !== null && _f !== void 0 ? _f : '',
                type: '',
                company_name: (_g = ((getWebsite === null || getWebsite === void 0 ? void 0 : getWebsite.baseName) === null ? '' : getWebsite === null || getWebsite === void 0 ? void 0 : getWebsite.baseName)) !== null && _g !== void 0 ? _g : '',
                industry_type: (_h = ((getWebsite === null || getWebsite === void 0 ? void 0 : getWebsite.CompanyType) === null ? '' : getWebsite === null || getWebsite === void 0 ? void 0 : getWebsite.CompanyType)) !== null && _h !== void 0 ? _h : '',
                customerId: (_k = (_j = data === null || data === void 0 ? void 0 : data.customerNumber) === null || _j === void 0 ? void 0 : _j.toString()) !== null && _k !== void 0 ? _k : '',
                messageId: (_l = data === null || data === void 0 ? void 0 : data.messageId) !== null && _l !== void 0 ? _l : ''
            };
            console.log('✅ Sending request to bot:', JSON.stringify(body, null, 2));
            const response = yield axios_1.default.post(process.env.semiChannel, body);
            console.log('✅ Response from nlp>>>>>>>>>>>', JSON.stringify(response.data, null, 2));
            return response;
        }
        catch (error) {
            console.error('❌ Error sending message:', error);
        }
    });
}
exports.AgentAssistEmailSms = AgentAssistEmailSms;
/**
 *
 * @param domainId
 * @returns
 */
const getKnowledgeBaseId = (domainId, knowledgeBaseId) => {
    return new Promise((resolve, reject) => __awaiter(void 0, void 0, void 0, function* () {
        try {
            db_1.mysqlPoolConnection.query('call get_detailsWebsiteScraper(?,?)', [
                domainId,
                knowledgeBaseId
            ], (err, result) => {
                if (err) {
                    reject(err);
                    console.log(err);
                }
                if (result && result.length > 0) {
                    resolve(result[0]);
                }
                else {
                    resolve([]);
                }
            });
        }
        catch (err) {
            reject(err);
        }
    }));
};
exports.getKnowledgeBaseId = getKnowledgeBaseId;
/**
 *
 * @param data
 * @returns
 */
const configurationDetails = (data) => {
    return new Promise((resolve, reject) => __awaiter(void 0, void 0, void 0, function* () {
        try {
            db_1.mysqlPoolConnection.query('call ccaas_getWhatsappBot(?,?)', [
                data === null || data === void 0 ? void 0 : data.agentNumber,
                data === null || data === void 0 ? void 0 : data.domainId
            ], (err, result) => {
                if (err) {
                    reject(err);
                    console.log('getBotDetail whatsapp err: ', err);
                }
                if (result && result.length > 0) {
                    resolve(result[0]);
                }
                else {
                    resolve([]);
                }
            });
        }
        catch (err) {
            console.log('getBotDetail catch err: ', err);
            reject(err);
        }
    }));
};
exports.configurationDetails = configurationDetails;
/**
 *
 * @param data
 * @returns
 */
const configurationEmailDetails = (data, value) => {
    return new Promise((resolve, reject) => __awaiter(void 0, void 0, void 0, function* () {
        try {
            db_1.mysqlPoolConnection.query('call get_knowledgeBasedId_assist(?,?,?)', [
                data === null || data === void 0 ? void 0 : data.agentNumber,
                data === null || data === void 0 ? void 0 : data.domainId,
                value
            ], (err, result) => {
                if (err) {
                    reject(err);
                    console.log('getBotDetail whatsapp err: ', err);
                }
                if (result && result.length > 0) {
                    resolve(result[0]);
                }
                else {
                    resolve([]);
                }
            });
        }
        catch (err) {
            console.log('getBotDetail catch err: ', err);
            reject(err);
        }
    }));
};
exports.configurationEmailDetails = configurationEmailDetails;
/**
 *
 * @param data
 * @param data1
 * @returns
 */
const configurationFbDetails = (data) => {
    return new Promise((resolve, reject) => __awaiter(void 0, void 0, void 0, function* () {
        try {
            db_1.mysqlPoolConnection.query('call ccaas_GetFacebookBot(?,?)', [
                data === null || data === void 0 ? void 0 : data.agentNumber,
                data === null || data === void 0 ? void 0 : data.domainId
            ], (err, result) => {
                if (err) {
                    reject(err);
                    console.log(err);
                }
                if (result && result.length > 0) {
                    resolve(result[0]);
                }
                else {
                    reject(err);
                }
            });
        }
        catch (err) {
            console.log(err);
            reject(err);
        }
    }));
};
exports.configurationFbDetails = configurationFbDetails;
/**
 *
 * @param data
 * @param data1
 * @returns
 */
const configurationInstaDetails = (data) => {
    return new Promise((resolve, reject) => __awaiter(void 0, void 0, void 0, function* () {
        try {
            db_1.mysqlPoolConnection.query('call ccaas_GetInstagramBot(?,?)', [
                data === null || data === void 0 ? void 0 : data.agentNumber,
                data === null || data === void 0 ? void 0 : data.domainId
            ], (err, result) => {
                if (err) {
                    reject(err);
                    console.log(err);
                }
                if (result && result.length > 0) {
                    resolve(result[0]);
                }
                else {
                    reject(err);
                }
            });
        }
        catch (err) {
            console.log(err);
            reject(err);
        }
    }));
};
exports.configurationInstaDetails = configurationInstaDetails;
/**
 * Get company details by domainId
 *
 * @param data
 * @returns Promise<any>
 */
const getCompanyDetails = (data) => {
    return new Promise((resolve, reject) => {
        var _a;
        try {
            db_1.mysqlPoolConnection.query('CALL up_GetCompanyDomain(?)', [(_a = data === null || data === void 0 ? void 0 : data.domainId) !== null && _a !== void 0 ? _a : 0], (errors, res) => {
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
                        resolve([]);
                    }
                }
            });
        }
        catch (error) {
            log_1.logger.error('getCompanyDetails err', error);
            reject(error);
        }
    });
};
exports.getCompanyDetails = getCompanyDetails;
// send nlp facebook decline
function getDeclineBot(data) {
    var _a;
    return __awaiter(this, void 0, void 0, function* () {
        try {
            const body = {
                domain_id: data === null || data === void 0 ? void 0 : data.domainId
            };
            log_1.logger.info('Sending request to bot:', JSON.stringify(body, null, 2));
            const response = yield axios_1.default.post((_a = process === null || process === void 0 ? void 0 : process.env) === null || _a === void 0 ? void 0 : _a.agentDecline, body);
            log_1.logger.info('Response from nlp>>>>>>>>>>>', JSON.stringify(response.data, null, 2));
            return response === null || response === void 0 ? void 0 : response.data;
        }
        catch (error) {
            log_1.logger.error('getFacebookBot err', error);
        }
    });
}
exports.getDeclineBot = getDeclineBot;
//# sourceMappingURL=socialmedia.js.map