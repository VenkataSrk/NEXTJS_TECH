import { mysqlPoolConnection } from "./db";
import { logger } from "./log";

/**
 *
 * @param data
 * @returns
 */
export const createState = (data: any) => {
    return new Promise((resolve, reject) => {
        try {
            mysqlPoolConnection.query('call ccaas_InsertUpdateAgentStateTimer(?,?,?)', [
                data?.domainId,
                data?.startTimestamp,
                data?.ext
            ], (err: any, result: any) => {
                logger.trace('ccaas_InsertUpdateAgentStateTimer', data)
                if (err) {
                    logger.error('ccaas_InsertUpdateAgentStateTimer', data, err)
                    reject(err);
                } else {
                    resolve(result);
                }
            });
        } catch (error) {
            logger.error('ccaas_InsertUpdateAgentStateTimer', data, error)
            reject(error);
        }
    });
};