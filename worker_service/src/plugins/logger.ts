import { logger } from './log';

export const loggerTrace = (name, payload, result) => {
  logger.trace(name, { payoad: payload, response: JSON.stringify(result) });
};

export const loggerError = (name, payload, error) => {
  logger.error(name, { payload, errorMessage: error });
};
