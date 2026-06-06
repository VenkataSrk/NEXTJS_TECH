import { logger } from '../plugins/log';

/**
 * Database Retry Utility
 *
 * Provides automatic retry logic for database operations that may encounter deadlocks
 * or other transient failures.
 */

export interface IRetryConfig {
  maxRetries?: number;
  retryDelayMs?: number;
  retryableErrorCodes?: string[];
  operationName?: string;
  logContext?: any;
}

const DEFAULT_CONFIG: Required<IRetryConfig> = {
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
export async function executeWithRetry<T>(
  queryExecutor: (attempt: number) => Promise<T>,
  config: IRetryConfig = {}
): Promise<T> {

  const finalConfig = { ...DEFAULT_CONFIG, ...config };
  const { maxRetries, retryDelayMs, retryableErrorCodes, operationName, logContext } = finalConfig;

  let lastError: any;

  for (let attempt = 1; attempt <= maxRetries; attempt = attempt + 1) {
    try {
      const result = await queryExecutor(attempt);

      // Log success if this was a retry
      if (attempt > 1) {
        logger.info(`${operationName} succeeded after ${attempt} attempts`, logContext);
      }

      return result;
    } catch (error: any) {
      lastError = error;

      // Check if this is a retryable error
      const isRetryable = retryableErrorCodes.indexOf(error.code) !== -1;
      const canRetry = isRetryable && attempt < maxRetries;

      if (canRetry) {
        const delay = retryDelayMs * attempt;
        logger.warn(
          `${operationName} failed with ${error.code}. Retrying (${attempt}/${maxRetries}) after ${delay}ms...`,
          { ...logContext, errorCode: error.code, errorMessage: error.message }
        );

        // Wait before retrying with exponential backoff
        await new Promise(resolve => setTimeout(resolve, delay));
      } else {
        // Either not retryable or max retries reached
        if (isRetryable) {
          logger.error(
            `${operationName} failed after ${maxRetries} retry attempts`,
            { ...logContext, error }
          );
        }
        throw error;
      }
    }
  }

  // This should never be reached, but TypeScript requires it
  throw lastError;
}

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
export async function executeMySQLWithRetry<T = any>(
  mysqlConnection: any,
  query: string,
  params: any[],
  config: IRetryConfig = {}
): Promise<T> {

  return executeWithRetry(
    () => new Promise<T>((resolve, reject) => {
      mysqlConnection.query(query, params, (err: any, result: any) => {
        if (err) {
          reject(err);
        } else {
          resolve(result);
        }
      });
    }),
    config
  );
}
