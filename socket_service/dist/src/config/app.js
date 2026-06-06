"use strict";
/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
exports.configs = void 0;
const dotenv_1 = require("dotenv");
const fastify_plugin_1 = __importDefault(require("fastify-plugin"));
(0, dotenv_1.config)();
exports.configs = {
    apiurl: (() => {
        return process.env.API_URL;
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
            connectTimeout: 10000,
            options: {
                encrypt: false,
            },
        };
    })(),
    redis_client: {
        host: process.env.BLPOP_REDIS_INSTANCE,
        port: 6379,
        password: process.env.BLPOP_REDIS_PASSWORD,
    },
    appname: process.env.APPLICATION_NAME,
    appversion: process.env.APP_VERSION,
    serverport: process.env.SERVER_PORT,
    servicepath: process.env.SERVICE_PATH,
    requestChannel: process.env.REQUESTCHANNEL,
    responseChannel: process.env.RESPONSECHANNEL,
    jwtsecret: 'sscret',
};
exports.default = (0, fastify_plugin_1.default)((app, opts, done) => {
    app.decorate('config', exports.configs);
    done();
});
//# sourceMappingURL=app.js.map