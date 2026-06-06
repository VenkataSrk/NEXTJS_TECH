import { logger } from './logger';

/**
 * Trace logger → for successful DB/API responses
 */
export const loggerTrace = (name: string, payload: any, result: any) => {
  logger.trace(name, { payload, response: JSON.stringify(result) });
};

  /**
   * Error logger → for DB/API errors
   */
export const loggerError = (name: string, payload: any, error: any) => {
  logger.error(name, { payload, errorMessage: error?.message ?? error });
};
