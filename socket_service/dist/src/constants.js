"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.updateInactivity = exports.isJSON = void 0;
const db_1 = require("./plugins/db");
function isJSON(str) {
    try {
        return JSON.parse(str) && !!str;
    }
    catch (e) {
        return false;
    }
}
exports.isJSON = isJSON;
function updateInactivity(domainId, ext, inActive) {
    const key = `${domainId}_inactivity`;
    const field = `${domainId}_${ext}`;
    db_1.pubsubRedisClient.hget(key, field)
        .then((res) => {
        if (res) {
            const presenceRes = isJSON(res) ? JSON.parse(res) : res;
            const data = Object.assign(Object.assign({}, presenceRes), { ccaasInactive: inActive });
            db_1.pubsubRedisClient.hset(key, field, JSON.stringify(data));
        }
        else {
            const data = {
                ccaasInactive: inActive,
            };
            db_1.pubsubRedisClient.hset(key, field, JSON.stringify(data));
        }
    })
        .catch((err) => console.log('update_inactivity_presence err>>>>', err));
}
exports.updateInactivity = updateInactivity;
//# sourceMappingURL=constants.js.map