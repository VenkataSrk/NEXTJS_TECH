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
exports.sendCallbackRequest = exports.sendConnectLiveAgentInstaNotification = exports.sendConnectLiveAgentInsta = exports.sendConnectLiveAgentFBNotification = exports.sendConnectLiveAgentFB = exports.chatDeflectionData = exports.sentimentalScoreData = exports.sendConnectLiveAgentNotification = exports.sendConnectLiveAgent = exports.sendMessageFacebook = exports.sendMessage = exports.replaceDynamicEndPoint = exports.getResponseData = exports.fetchApi = exports.hitEndpoint = void 0;
const axios_1 = __importDefault(require("axios"));
const webChat_dao_1 = require("./dao/webChat.dao");
const hitEndpoint = (method, url, headers, body) => __awaiter(void 0, void 0, void 0, function* () {
    try {
        switch (method) {
            case 'post': {
                return yield axios_1.default.post(url, body, {
                    headers,
                });
            }
            case 'put': {
                return yield axios_1.default.put(url, body, {
                    headers,
                });
            }
            case 'delete': {
                return yield axios_1.default.delete(url, {
                    headers,
                });
            }
            default: {
                return yield axios_1.default.get(url, {
                    headers,
                });
            }
        }
    }
    catch (error) {
        console.log('error-----------', error);
        return error;
    }
});
exports.hitEndpoint = hitEndpoint;
const fetchApi = ({ urlEndPoint, urlMethod, urlBody, urlHeaders, inputParams }) => __awaiter(void 0, void 0, void 0, function* () {
    try {
        const method = urlMethod !== null && urlMethod !== void 0 ? urlMethod : 'get';
        const url = urlEndPoint;
        const headers = urlHeaders !== null && urlHeaders !== void 0 ? urlHeaders : [];
        const body = urlBody !== null && urlBody !== void 0 ? urlBody : [];
        const formattedHeader = {};
        const formattedBody = {};
        const finalObj = {};
        for (let i = 0; i < (inputParams ? inputParams.length : 0); i += 1) {
            Object.assign(finalObj, inputParams[i]);
        }
        console.log(inputParams, 'inputParamsinputParamsinputParams');
        headers.forEach((header) => {
            if (Object.keys(header).length > 0) {
                formattedHeader[header.key] = header.value;
            }
        });
        formattedHeader['Content-Type'] = 'application/json';
        body.forEach((data) => {
            if (Object.keys(data).length > 0) {
                const regex_symbols = /\${context\.(\w+)}/;
                const text = (data === null || data === void 0 ? void 0 : data.value) !== undefined ? data === null || data === void 0 ? void 0 : data.value : '';
                const test = text.trim().match(regex_symbols);
                const result = test ? test[1] : null;
                const final = finalObj[result] !== undefined ? finalObj[result] : '';
                formattedBody[data.key] = test ? final : text;
                console.log(finalObj, result, finalObj[result], 'resultresultresultresult88');
            }
        });
        const results = yield (0, exports.hitEndpoint)(method, url, formattedHeader, formattedBody);
        return { results };
    }
    catch (error) {
        return { error };
    }
});
exports.fetchApi = fetchApi;
const getResponseData = (response) => {
    let responseData = response;
    while ((responseData === null || responseData === void 0 ? void 0 : responseData.data) || (responseData === null || responseData === void 0 ? void 0 : responseData.results) || (responseData === null || responseData === void 0 ? void 0 : responseData.result) || (responseData === null || responseData === void 0 ? void 0 : responseData.topupAmountResRes)) {
        if (responseData === null || responseData === void 0 ? void 0 : responseData.data)
            responseData = responseData === null || responseData === void 0 ? void 0 : responseData.data;
        if (responseData === null || responseData === void 0 ? void 0 : responseData.result)
            responseData = responseData === null || responseData === void 0 ? void 0 : responseData.result;
        if (responseData === null || responseData === void 0 ? void 0 : responseData.results)
            responseData = responseData === null || responseData === void 0 ? void 0 : responseData.results;
        if (responseData === null || responseData === void 0 ? void 0 : responseData.topupAmountResRes)
            responseData = responseData === null || responseData === void 0 ? void 0 : responseData.topupAmountResRes;
    }
    if (responseData) {
        return responseData;
    }
};
exports.getResponseData = getResponseData;
const replaceDynamicEndPoint = (url, inputParams) => {
    const contextApiId = url.match(/\${(.*?)}/)[1].replace('context.', '');
    const contextApiIdOrg = url.match(/\${(.*?)}/)[1];
    const index = inputParams.findIndex((item) => Object.keys(item).includes(contextApiId));
    if (index !== -1) {
        const updatedUrl = url.replace(`\${${contextApiIdOrg}}`, inputParams[index][contextApiId]);
        return updatedUrl;
    }
    return url;
};
exports.replaceDynamicEndPoint = replaceDynamicEndPoint;
// send message
function sendMessage(req) {
    var _a;
    return __awaiter(this, void 0, void 0, function* () {
        try {
            const getId = yield (0, webChat_dao_1.getBotDetail)(req);
            const body = {
                id: (_a = getId[0]) === null || _a === void 0 ? void 0 : _a.purchasedat,
                from: req === null || req === void 0 ? void 0 : req.agentNumber,
                to: `+${req === null || req === void 0 ? void 0 : req.customerNumber}`,
                content: 'Our agents are currently busy. Please feel free to leave a message with a brief summary of your inquiry along with your contact information, or you can get in touch with us at a later time.'
            };
            const response = yield axios_1.default.post(process.env.whatsappOut, body);
            return body;
        }
        catch (error) {
            console.error('Error sending message:', error);
        }
    });
}
exports.sendMessage = sendMessage;
// send message facebook
function sendMessageFacebook(req) {
    return __awaiter(this, void 0, void 0, function* () {
        try {
            const body = {
                domainId: req === null || req === void 0 ? void 0 : req.domainId,
                customerId: req === null || req === void 0 ? void 0 : req.customerNumber,
                pageId: req === null || req === void 0 ? void 0 : req.agentNumber,
                message: 'Our agents are currently not available. Please feel free to leave a message with a brief summary of your inquiry along with your contact information, or you can get in touch with us at a later time.'
            };
            const response = yield axios_1.default.post(process.env.FacebookOut, body);
            return body;
        }
        catch (error) {
            console.error('Error sending message:', error);
            // You can handle the error here, e.g., log it or return a specific response
        }
    });
}
exports.sendMessageFacebook = sendMessageFacebook;
// send message connect live agent
function sendConnectLiveAgent(req) {
    var _a;
    return __awaiter(this, void 0, void 0, function* () {
        try {
            const getId = yield (0, webChat_dao_1.getBotDetail)(req);
            const body = {
                id: (_a = getId[0]) === null || _a === void 0 ? void 0 : _a.purchasedat,
                from: req === null || req === void 0 ? void 0 : req.agentNumber,
                to: `+${req === null || req === void 0 ? void 0 : req.customerNumber}`,
                content: `Your conversation has been transferred to ${req === null || req === void 0 ? void 0 : req.agentName}`
            };
            const response = yield axios_1.default.post(process.env.whatsappOut, body);
            return body;
        }
        catch (error) {
            console.error('Error sending message:', error);
        }
    });
}
exports.sendConnectLiveAgent = sendConnectLiveAgent;
// send message connect live agent
function sendConnectLiveAgentNotification(req) {
    var _a;
    return __awaiter(this, void 0, void 0, function* () {
        try {
            const getId = yield (0, webChat_dao_1.getBotDetail)(req);
            const body = {
                id: (_a = getId[0]) === null || _a === void 0 ? void 0 : _a.purchasedat,
                from: req === null || req === void 0 ? void 0 : req.agentNumber,
                to: `+${req === null || req === void 0 ? void 0 : req.customerNumber}`,
                content: 'Connecting to live agent...'
            };
            yield axios_1.default.post(process.env.whatsappOut, body);
            return body;
        }
        catch (error) {
            console.error('Error sending message:', error);
            // You can handle the error here, e.g., log it or return a specific response
        }
    });
}
exports.sendConnectLiveAgentNotification = sendConnectLiveAgentNotification;
// senti mental Score
function sentimentalScoreData(data) {
    var _a, _b, _c;
    return __awaiter(this, void 0, void 0, function* () {
        try {
            const body = {
                HistoryId: data === null || data === void 0 ? void 0 : data.historyId,
                ChatDurationHistoryId: (_a = data === null || data === void 0 ? void 0 : data.ChatDurationHistoryId) !== null && _a !== void 0 ? _a : 0,
                channelType: data === null || data === void 0 ? void 0 : data.channelType,
                domainId: data === null || data === void 0 ? void 0 : data.domainId,
                ext: (_b = parseInt(data === null || data === void 0 ? void 0 : data.ext, 10)) !== null && _b !== void 0 ? _b : 0
            };
            return yield axios_1.default.post((_c = process === null || process === void 0 ? void 0 : process.env) === null || _c === void 0 ? void 0 : _c.Sentimental, body);
        }
        catch (error) {
            console.error('Error sending message:', error);
            // You can handle the error here, e.g., log it or return a specific response
        }
    });
}
exports.sentimentalScoreData = sentimentalScoreData;
// chat deflection qa
function chatDeflectionData(data) {
    var _a, _b, _c;
    return __awaiter(this, void 0, void 0, function* () {
        try {
            const body = {
                HistoryId: data === null || data === void 0 ? void 0 : data.historyId,
                ChatDurationHistoryId: (_a = data === null || data === void 0 ? void 0 : data.ChatDurationHistoryId) !== null && _a !== void 0 ? _a : 0,
                channelType: data === null || data === void 0 ? void 0 : data.channelType,
                domainId: data === null || data === void 0 ? void 0 : data.domainId,
                ext: (_b = data === null || data === void 0 ? void 0 : data.ext) !== null && _b !== void 0 ? _b : 0
            };
            return yield axios_1.default.post((_c = process === null || process === void 0 ? void 0 : process.env) === null || _c === void 0 ? void 0 : _c.Deflection, body);
        }
        catch (error) {
            console.error('Error sending message:', error);
            // You can handle the error here, e.g., log it or return a specific response
        }
    });
}
exports.chatDeflectionData = chatDeflectionData;
// send message connect live agent
function sendConnectLiveAgentFB(data) {
    return __awaiter(this, void 0, void 0, function* () {
        try {
            const body = {
                domainId: data === null || data === void 0 ? void 0 : data.domainId,
                customerId: data === null || data === void 0 ? void 0 : data.customerNumber,
                pageId: data === null || data === void 0 ? void 0 : data.agentNumber,
                type: 'text',
                message: `Your conversation has been transferred to ${data === null || data === void 0 ? void 0 : data.agentName}`
            };
            const response = yield axios_1.default.post(process.env.FacebookOut, body);
            return body;
        }
        catch (error) {
            console.error('Error sending message:', error);
        }
    });
}
exports.sendConnectLiveAgentFB = sendConnectLiveAgentFB;
// send message connect live agent
function sendConnectLiveAgentFBNotification(data) {
    return __awaiter(this, void 0, void 0, function* () {
        try {
            const body = {
                domainId: data === null || data === void 0 ? void 0 : data.domainId,
                customerId: data === null || data === void 0 ? void 0 : data.customerNumber,
                pageId: data === null || data === void 0 ? void 0 : data.agentNumber,
                type: 'text',
                message: 'Connecting to live agent...'
            };
            const response = yield axios_1.default.post(process.env.FacebookOut, body);
            return body;
        }
        catch (error) {
            console.error('Error sending message:', error);
        }
    });
}
exports.sendConnectLiveAgentFBNotification = sendConnectLiveAgentFBNotification;
// send message connect live agent
function sendConnectLiveAgentInsta(data) {
    return __awaiter(this, void 0, void 0, function* () {
        try {
            const body = {
                domainId: data === null || data === void 0 ? void 0 : data.domainId,
                customerId: data === null || data === void 0 ? void 0 : data.customerNumber,
                pageId: data === null || data === void 0 ? void 0 : data.agentNumber,
                type: 'text',
                message: `Your conversation has been transferred to ${data === null || data === void 0 ? void 0 : data.agentName}`
            };
            const response = yield axios_1.default.post(process.env.InstagramOut, body);
            return body;
        }
        catch (error) {
            console.error('Error sending message:', error);
        }
    });
}
exports.sendConnectLiveAgentInsta = sendConnectLiveAgentInsta;
// send message connect live agent
function sendConnectLiveAgentInstaNotification(data) {
    return __awaiter(this, void 0, void 0, function* () {
        try {
            const body = {
                domainId: data === null || data === void 0 ? void 0 : data.domainId,
                customerId: data === null || data === void 0 ? void 0 : data.customerNumber,
                pageId: data === null || data === void 0 ? void 0 : data.agentNumber,
                type: 'text',
                message: 'Connecting to live agent...'
            };
            const response = yield axios_1.default.post(process.env.InstagramOut, body);
            return body;
        }
        catch (error) {
            console.error('Error sending message:', error);
        }
    });
}
exports.sendConnectLiveAgentInstaNotification = sendConnectLiveAgentInstaNotification;
// send Button Link for callback
function sendCallbackRequest(req, callbackUrl) {
    var _a, _b;
    return __awaiter(this, void 0, void 0, function* () {
        try {
            const getId = yield (0, webChat_dao_1.getBotDetail)(req);
            const body = {
                id: (_a = getId[0]) === null || _a === void 0 ? void 0 : _a.purchasedat,
                from: req === null || req === void 0 ? void 0 : req.agentNumber,
                to: `+${req === null || req === void 0 ? void 0 : req.customerNumber}`,
                templateName: process.env.templateNameCallBack,
                type: 'cta_template_body_buttons',
                buttonUrl: callbackUrl,
            };
            yield axios_1.default.post((_b = process === null || process === void 0 ? void 0 : process.env) === null || _b === void 0 ? void 0 : _b.sendCallbackRequest, body);
            return body;
        }
        catch (error) {
            console.error('Error sending message:', error === null || error === void 0 ? void 0 : error.response);
            // You can handle the error here, e.g., log it or return a specific response
        }
    });
}
exports.sendCallbackRequest = sendCallbackRequest;
//# sourceMappingURL=fetch_api.js.map