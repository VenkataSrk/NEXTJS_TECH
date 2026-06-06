"use strict";
/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */
Object.defineProperty(exports, "__esModule", { value: true });
exports.invalidCredentials = exports.missingAuthenticationToken = void 0;
function missingAuthenticationToken(message) {
    return {
        message,
        statusCode: 401,
        error: 'unauthorized',
    };
}
exports.missingAuthenticationToken = missingAuthenticationToken;
function invalidCredentials(message) {
    return {
        message,
        statusCode: 403,
        error: 'forbidden',
    };
}
exports.invalidCredentials = invalidCredentials;
//# sourceMappingURL=response-builders.js.map