/**
 * @createdBy Selvakani
 * @createdOn 27th April 2021
 */


//10.22.2.12 staging
//10.150.0.72 live

const mongoose = require('mongoose')
const mysql = require('mysql')

import { config } from 'dotenv';
import { Emitter } from "@socket.io/redis-emitter";
import Redis from 'ioredis';
//var redis = require("redis");

//const Redis = require("ioredis");
var redis = require("redis");
config();

console.log("process.env.NODE_ENV >>>>>>>", process.env.NODE_ENV);
export const configuration = {
    server: {
        host: process.env.HOST || 'localhost',
        port: process.env.PORT || 8080,
    },
    jwt_secret: process.env.JWT_SECRET || 'sscret',
    cors_options: {
        origin: '*',
        preflightContinue: false,
        optionsSuccessStatus: 204,
        credentials: false,
        exposedHeaders: null,
        allowedHeaders: null,
        maxAge: null,
        preflight: true,
        hideOptionsRoute: true,

    },
    redis_client: (() => {
        if (process.env.NODE_ENV === 'gsproduction') {
            return {
                host: process.env.GS_PRODUCTION_REDIS,
                port: 6379,
            };
        } else if (process.env.NODE_ENV === 'thproduction') {
            return {
                host: process.env.TH_PRODUCTION_REDIS,
                port: 6379,
                no_ready_check: true,
                auth_pass: process.env.PROD_REDIS_AUTH
            };

        } else if (process.env.NODE_ENV === 'qastaging') {
            return {
                host: process.env.QA_REDIS,
                port: 6379,
                no_ready_check: true,
                auth_pass: process.env.QAREDIS_AUTH
            };

        } else {
            return {
                host: process.env.STAGING_REDIS,
                port: 6379,
                no_ready_check: true,
                auth_pass: process.env.STAGING_REDIS_AUTH
            };;
        }


    })(),

    chat_redis_client: (() => {
        if (process.env.NODE_ENV === 'gsproduction') {
            return {
                host: process.env.GS_PRODUCTION_REDIS,
                port: 6379,
            };
        } else if (process.env.NODE_ENV === 'thproduction') {

            return {
                host: process.env.CHAT_TH_PRODUCTION_REDIS,
                port: 6379,
                no_ready_check: true,
                auth_pass: process.env.CHAT_PROD_REDIS_AUTH
            };

            /*  return [
                 {
                     port: 6380,
                     host: '10.150.3.171',
                 },
                 {
                     port: 6380,
                     host: '10.150.3.172',
                 }, {
                     port: 6380,
                     host: '10.150.3.173',
                 },
                 {
                     port: 6380,
                     host: '10.150.3.175',
                 },
                 {
                     port: 6380,
                     host: '10.171.3.47',
                 },
                 {
                     port: 6380,
                     host: '10.171.3.45',
                 },
                 {
                     port: 6380,
                     host: '10.150.3.174',
                 },
                 {
                     port: 6380,
                     host: '10.171.3.42',
                 },
                 {
                     port: 6380,
                     host: '10.171.3.43',
                 },
                 {
                     port: 6380,
                     host: '10.171.3.44',
                 },
                 {
                     port: 6380,
                     host: '10.150.3.176',
                 },
                 {
                     port: 6380,
                     host: '10.171.3.46',
                 }
             ] */

        }/*  else if (process.env.NODE_ENV === 'qastaging') {
            return [
                {
                    port: 6380,
                    host: "10.150.0.72",
                },
                {
                    port: 6379,
                    host: "10.150.0.72",
                }, {
                    port: 6382,
                    host: "10.150.0.72",
                },
                {
                    port: 6381,
                    host: "10.150.0.72",
                },
                {
                    port: 6384,
                    host: "10.150.0.72",
                },
                {
                    port: 6383,
                    host: "10.150.0.72",
                },
            ]

        } */
        else if (process.env.NODE_ENV === 'qastaging') {

            return {
                host: process.env.QA_CHAT_REDIS,
                port: 6379,
                no_ready_check: true,
                auth_pass: process.env.QA_CHAT_REDIS_AUTH
            };
        }
        else {
            return {
                host: process.env.STAGING_REDIS,
                port: 6379,
                no_ready_check: true,
                auth_pass: process.env.STAGING_REDIS_AUTH
            };;
        }


    })(),
    /* {
        host: process.env.redisHOST || '10.150.0.72',
        port: 6379,
        no_ready_check: true,
        auth_pass: "A9t1QNEMsWL6+msYxhRxL8hFYoA"
    }, */
    meeting: {
        host: process.env.METTING_HOST || 'localhost',
        port: 5003
    },

    restAPIURL: (() => {
        if (process.env.NODE_ENV === 'gsproduction' || process.env.NODE_ENV === 'thproduction') {
            return process.env.PRODUCTION_REST_API;
        } else if (process.env.NODE_ENV === 'qastaging') {
            return process.env.QA_REST_API;
        }

        return process.env.STAGING_REST_API;
    })(),

    restCHATAPIURL: (() => {
        if (process.env.NODE_ENV === 'gsproduction' || process.env.NODE_ENV === 'thproduction') {
            return process.env.PRODUCTION_CHAT_REST_API;
        } else if (process.env.NODE_ENV === 'qastaging') {
            return process.env.QA_CHAT_REST_API;
        }

        return process.env.STAGING_CHAT_REST_API;
    })(),
    mongo_db: (() => {
        if (process.env.NODE_ENV === 'qastaging') {
            return process.env.QA_MONGO_UNIFIEDRING_PROD_URL;
        } else if (process.env.NODE_ENV === "staging") {
            return process.env.MONGO_UNIFIEDRING_STAGE_URL;
        }

        return process.env.MONGO_UNIFIEDRING_PROD_URL;
    })(),
    mysql_config: (() => {
        if (process.env.NODE_ENV === 'qastaging') {
            return {
                host: '10.171.9.131',
                user: 'JAVAteam',
                password: 'JAVAteam@123',
                database: 'unifiedring',
                connectionLimit: 10,
                options: {
                    encrypt: false,
                },
                ssl: {
                    rejectUnauthorized: false
                }
            };
        } else if (process.env.NODE_ENV === "staging") {
            return {
                host: '10.171.9.131',
                user: 'JAVAteam',
                password: 'JAVAteam@123',
                database: 'unifiedring',
                connectionLimit: 10,
                options: {
                    encrypt: false,
                },
                ssl: {
                    rejectUnauthorized: false
                }
            };
        }
        return {
            host: '10.150.0.53',
            user: 'JAVAteam',
            password: 'JAVAteam@123',
            database: 'unifiedring',
            connectionLimit: 10,
            options: {
                encrypt: false,
            },
            ssl: {
                rejectUnauthorized: false
            }
        };
    })(),
    ccaas_mysql_config:(()=>{
        if (process.env.NODE_ENV === 'qastaging') {
            return {


                        host: '10.150.0.176',
                        user: 'ccaasdev',
                        password: 'ccAAsdev@435',
                        database: 'ccaas2023',
                        port: 3306,
                        insecureAuth: true,
                        // options: {
                        //   encrypt: false,
                        // },

            };
        } else if (process.env.NODE_ENV === "staging") {
            return {
                host: '10.150.0.176',
                user: 'ccaasdev',
                password: 'ccAAsdev@435',
                database: 'ccaas2023',
                port: 3306,
                insecureAuth: true,
                // options: {
                //   encrypt: false,
                // },
            };
        }
        return {
            host: '10.150.3.70',
            user: 'ccaasdev',
            password: 'ccAAsdev@435',
            database: 'ccaas2023',
            port: 3306,
            insecureAuth: true,
            // options: {
            //   encrypt: false,
            // },
        };
    })(),
    hubspot: {
        apikey: '84e21db2-f51a-4b78-8950-bd3ec7ed8104',
        host: 'https://api.hubapi.com/crm/v3/objects/contacts/search',
    },

    videoRecordFileURL: (() => {
        if (process.env.NODE_ENV === 'thproduction') {
            return process.env.PRODUCTION_RECORDFILE_URL;
        } else if (process.env.NODE_ENV === "qastaging") {
            return process.env.QA_RECORDFILE_URL;
        }

        return process.env.PRODUCTION_RECORDFILE_URL;
    })(),
    redisCluster: (() => {
        if (process.env.NODE_ENV === 'qastaging') {

            return [
                {
                    port: 6380,
                    host: "10.150.0.72",
                },
                {
                    port: 6379,
                    host: "10.150.0.72",
                }, {
                    port: 6382,
                    host: "10.150.0.72",
                },
                {
                    port: 6381,
                    host: "10.150.0.72",
                },
                {
                    port: 6384,
                    host: "10.150.0.72",
                },
                {
                    port: 6383,
                    host: "10.150.0.72",
                },
            ];
        } else if (process.env.NODE_ENV === "staging") {
            return process.env.MONGO_UNIFIEDRING_STAGE_URL;
        }

        /*  return [
             {
                 port: 6380,
                 host: '10.150.3.171',
             },
             {
                 port: 6380,
                 host: '10.150.3.172',
             }, {
                 port: 6380,
                 host: '10.150.3.173',
             },
             {
                 port: 6380,
                 host: '10.150.3.175',
             },
             {
                 port: 6380,
                 host: '10.171.3.47',
             },
             {
                 port: 6380,
                 host: '10.171.3.45',
             },
             {
                 port: 6380,
                 host: '10.150.3.174',
             },
             {
                 port: 6380,
                 host: '10.171.3.42',
             },
             {
                 port: 6380,
                 host: '10.171.3.43',
             },
             {
                 port: 6380,
                 host: '10.171.3.44',
             },
             {
                 port: 6380,
                 host: '10.150.3.176',
             },
             {
                 port: 6380,
                 host: '10.171.3.46',
             }
         ] */
        return [
            {
                port: 6380,
                host: "10.150.0.30",
            },
            {
                port: 6380,
                host: "10.150.0.28",
            }, {
                port: 6380,
                host: "10.150.0.26",
            },
            {
                port: 6380,
                host: "10.171.1.65",
            },
            {
                port: 6380,
                host: "10.150.0.230",
            },
            {
                port: 6380,
                host: "10.150.0.185",
            },
            {
                port: 6380,
                host: "10.171.1.155",
            },
            {
                port: 6380,
                host: "10.22.3.180",
            },
            {
                port: 6380,
                host: "10.22.8.86",
            }
        ];
    })(),

    sttRedis: (() => {
        if (process.env.NODE_ENV === 'qastaging') {
            return {
                host: process.env.QA_REDIS,
                port: 6379,
                no_ready_check: true,
                auth_pass: process.env.QAREDIS_AUTH
            };
        }
        else if (process.env.NODE_ENV === 'staging') {
            return {
                host: process.env.STT_REDIS,
                port: 6379,
                auth_pass: process.env.STT_REDIS_AUTH,
                no_ready_check: true
            };
        }
        else {
            return {
                host: process.env.TH_PRODUCTION_REDIS,
                port: 6379,
                no_ready_check: true,
                auth_pass: process.env.PROD_REDIS_AUTH
            };
        }
    })(),

};

console.log("MongoDB >>>>>", configuration.mongo_db)
console.log("RestAPIURL >>>>>", configuration.restAPIURL)
console.log("RedisDB >>>>>", configuration.redis_client)

console.log("configuration.mysql_config >>>>>", configuration.mysql_config)
// export const unifiedRingMongoDB = mongoose.createConnection(configuration.mongo_db, { useUnifiedTopology: true, useNewUrlParser: true },
//     function (err: any) {
//         if (err) {
//             throw err
//         }
//         console.log('MongoDatabase connected');

//     });
export const worktualMySQl = mysql.createPool(configuration.mysql_config,
    function (err: any) {
        if (err) {
            throw err
        }
        console.log('Mysql connected');
    });

export const worktualMySQlCcaas = mysql.createPool(configuration.ccaas_mysql_config,
        function (err: any) {
            if (err) {
                throw err
            }
            console.log('CcaasMysql connected');
        });

let redisConfig: any = {
    host: configuration.chat_redis_client.host,
    port: configuration.redis_client.port,
};
redisConfig.no_ready_check = configuration.redis_client.no_ready_check;
redisConfig.auth_pass = configuration.chat_redis_client.auth_pass;


console.log("chat Server details", redisConfig);
const subscriber = redis.createClient(redisConfig);

//console.log("CHAT REDIS >>>>>>>", configuration.chat_redis_client);
// let nodes = configuration.chat_redis_client;
// const pub = new Redis.Cluster(nodes);

let CBConfig: any = {
    host: configuration.redis_client.host,
    port: configuration.redis_client.port,
    password: configuration.redis_client.auth_pass
};
let sttRedisConfig: any = {
    host: configuration.sttRedis.host,
    port: configuration.sttRedis.port,
    password: configuration.sttRedis.auth_pass
}

export const NLPRedisClient = new Redis(CBConfig);
export const CBRedisClient = new Redis(sttRedisConfig);

export const ioredis = new Emitter(subscriber);