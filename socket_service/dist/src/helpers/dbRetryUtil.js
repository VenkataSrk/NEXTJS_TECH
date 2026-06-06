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
Object.defineProperty(exports, "__esModule", { value: true });
exports.executeMySQLWithRetry = exports.executeWithRetry = void 0;
const log_1 = require("../plugins/log");
const DEFAULT_CONFIG = {
    maxRetries: 3,
    retryDelayMs: 50,
    retryableErrorCodes: ['ER_LOCK_DEADLOCK', 'ER_LOCK_WAIT_TIMEOUT'],
    operationName: 'Database Operation',
    logContext: {}
};
/**
 * Execute a database query with automatic retry logic for transient failures
 *
 * @param queryExecutor - Async function that executes the database query
 * @param config - Retry configuration options
 * @returns Promise with query result
 *
 * @example
 * const result = await executeWithRetry(
 *   () => new Promise((resolve, reject) => {
 *     mysqlPool.query('CALL sp_name(?)', [param], (err, result) => {
 *       err ? reject(err) : resolve(result);
 *     });
 *   }),
 *   { operationName: 'UpdateDisposition', logContext: { sessionId: '123' } }
 * );
 */
function executeWithRetry(queryExecutor, config = {}) {
    return __awaiter(this, void 0, void 0, function* () {
        const finalConfig = Object.assign(Object.assign({}, DEFAULT_CONFIG), config);
        const { maxRetries, retryDelayMs, retryableErrorCodes, operationName, logContext } = finalConfig;
        let lastError;
        for (let attempt = 1; attempt <= maxRetries; attempt = attempt + 1) {
            try {
                const result = yield queryExecutor(attempt);
                // Log success if this was a retry
                if (attempt > 1) {
                    log_1.logger.info(`${operationName} succeeded after ${attempt} attempts`, logContext);
                }
                return result;
            }
            catch (error) {
                lastError = error;
                // Check if this is a retryable error
                const isRetryable = retryableErrorCodes.indexOf(error.code) !== -1;
                const canRetry = isRetryable && attempt < maxRetries;
                if (canRetry) {
                    const delay = retryDelayMs * attempt;
                    log_1.logger.warn(`${operationName} failed with ${error.code}. Retrying (${attempt}/${maxRetries}) after ${delay}ms...`, Object.assign(Object.assign({}, logContext), { errorCode: error.code, errorMessage: error.message }));
                    // Wait before retrying with exponential backoff
                    yield new Promise(resolve => setTimeout(resolve, delay));
                }
                else {
                    // Either not retryable or max retries reached
                    if (isRetryable) {
                        log_1.logger.error(`${operationName} failed after ${maxRetries} retry attempts`, Object.assign(Object.assign({}, logContext), { error }));
                    }
                    throw error;
                }
            }
        }
        // This should never be reached, but TypeScript requires it
        throw lastError;
    });
}
exports.executeWithRetry = executeWithRetry;
/**
 * Wrapper for MySQL query execution with retry logic
 *
 * @param mysqlConnection - MySQL connection pool
 * @param query - SQL query string
 * @param params - Query parameters
 * @param config - Retry configuration
 * @returns Promise with query result
 *
 * @example
 * const result = await executeMySQLWithRetry(
 *   mysqlPoolConnection,
 *   'CALL ccaas_UpdateSessionDisposition_details(?,?,?,?,?,?,?,?,?)',
 *   [domainId, sessionId, summary, did, afterCallWorkTime, coid, subject, markAsRead, ext],
 *   { operationName: 'UpdateCallDisposition', logContext: { sessionId } }
 * );
 */
function executeMySQLWithRetry(mysqlConnection, query, params, config = {}) {
    return __awaiter(this, void 0, void 0, function* () {
        return executeWithRetry(() => new Promise((resolve, reject) => {
            mysqlConnection.query(query, params, (err, result) => {
                if (err) {
                    reject(err);
                }
                else {
                    resolve(result);
                }
            });
        }), config);
    });
}
exports.executeMySQLWithRetry = executeMySQLWithRetry;
//# sourceMappingURL=dbRetryUtil.js.map