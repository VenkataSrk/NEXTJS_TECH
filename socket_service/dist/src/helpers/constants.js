"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.splitFirst = exports.insertQueuedTimeForChat = exports.channelTypeCode = exports.ROLES = void 0;
const webChat_dao_1 = require("../../src/dao/webChat.dao");
exports.ROLES = {
    OWNER_ROLE_ID: '1',
    SUB_OWNER_ROLE_ID: '2',
    ADMIN_ROLE_ID: '3',
    MEMBER_ROLE_ID: '4',
    SUPERVISOR_ROLE_ID: '5',
    AGENT_ROLE_ID: '6',
};
exports.channelTypeCode = {
    Call: 2003001,
    Chat: 2003002,
    Email: 2003003,
    SMS: 2003004,
    Whatsapp: 2003005,
    Facebook: 2003006,
    Instagram: 2003007
};
const insertQueuedTimeForChat = (data) => {
    var _a, _b, _c, _d, _e, _f;
    try {
        const currentTime = Math.floor(Date.now() / 1000);
        const queuedTime = (_b = (_a = data === null || data === void 0 ? void 0 : data[0]) === null || _a === void 0 ? void 0 : _a.queuedTime) !== null && _b !== void 0 ? _b : 0;
        const queueDurationSeconds = currentTime - queuedTime;
        const bodydata = {
            domainId: (_d = (_c = data[0]) === null || _c === void 0 ? void 0 : _c.channelId) === null || _d === void 0 ? void 0 : _d.domain_id,
            ChatHistoryId: (_f = (_e = data[0]) === null || _e === void 0 ? void 0 : _e.chatHistoryId) !== null && _f !== void 0 ? _f : 0,
            queueDuration: queueDurationSeconds !== null && queueDurationSeconds !== void 0 ? queueDurationSeconds : 0,
        };
        console.log('<<<<<bodydata>>>>>', bodydata);
        (bodydata === null || bodydata === void 0 ? void 0 : bodydata.ChatHistoryId) ? (0, webChat_dao_1.createChatSession)(bodydata) : '';
    }
    catch (err) {
        console.log(err);
    }
};
exports.insertQueuedTimeForChat = insertQueuedTimeForChat;
const splitFirst = (str, sep = '') => {
    try {
        if (!str)
            return ['', ''];
        if ((str === null || str === void 0 ? void 0 : str.indexOf(sep)) === 0) {
            return ['', str.slice(sep === null || sep === void 0 ? void 0 : sep.length)];
        }
        return ['', str];
    }
    catch (error) {
        return ['', str];
    }
};
exports.splitFirst = splitFirst;
//# sourceMappingURL=constants.js.map