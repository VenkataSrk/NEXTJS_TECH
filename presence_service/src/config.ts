/**
 * @createdBy <Jayaselva M>
 * @createdOn
 */

import { config } from 'dotenv';
const env = process.env.NODE_ENV || 'qa';
console.log(`✅ Loaded environment: ${env}`);
config({ path: `.env.${env}` });

// config();
export interface IConfig {
    appname: string | undefined;
    serverport: string | undefined;
    requestChannel: string | undefined;
    responseChannel: string | undefined;
    redisClient: {
        host: string | undefined;
        password: string | undefined;
        port: number | undefined;
    };
    socketRedisClient: {
        host: string | undefined;
        password: string | undefined;
        port: number | undefined;
    };
    tokenRedisStore: {
        host: string | undefined;
        password: string | undefined;
        port: number | undefined;
    };
    sqlCcaasDB: {
        user: string | undefined;
        host: string | undefined;
        password: string | undefined;
        database: string | undefined;
        port: number | undefined;
        options: {
            encrypt: boolean;
        };
    };
}

export const configs: IConfig = {
    appname: process.env.APPLICATION_NAME,
    serverport: process.env.PRESENCE_PORT,
    requestChannel: process.env.PRESENCE_CHANNEL_REQUEST,
    responseChannel: process.env.PRESENCE_CHANNEL_RESPONSE,
    redisClient: (() => {
        return {
            host: process.env.BLPOP_REDIS_INSTANCE,
            password: process.env.BLPOP_REDIS_PASSWORD,
            port: 6379
        };
    })(),
    socketRedisClient: (() => {
        return {
            host: process.env.SOCKET_REDIS_INSTANCE,
            password: process.env.SOCKET_REDIS_PASSWORD,
            port: 6379
        };
    })(),
    tokenRedisStore: (() => {
        return {
            host: process.env.TOKEN_REDIS_STORE_IP,
            password: process.env.TOKEN_REDIS_STORE_PASSWORD,
            port: 6379
        };
    })(),
    sqlCcaasDB: (() => {
        return {
            host: process.env.MYSQL_HOST,
            user: process.env.MYSQL_USER,
            password: process.env.MYSQL_PASSWORD,
            database: process.env.MYSQL_DB,
            port: process.env.MYSQL_PORT,
            charset: 'utf8mb4',
            insecureAuth: true,
            options: {
                encrypt: false,
            },
        } as any;
    })(),
};
