/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */

import { IncomingMessage, Server, ServerResponse } from 'http';

import { config } from 'dotenv';
import { FastifyInstance } from 'fastify';
import fp from 'fastify-plugin';
import { swagger } from '../helpers/constants';

const nodeEnv = process.env.NODE_ENV || 'qa';
const envFile = `.env.${nodeEnv}`;

console.log(`🚀 Loaded config for ENV: ${nodeEnv}`);
console.log(`👉 Using env file: ${envFile}`);

config({ path: envFile });
export interface IConfig {
  path: any;
  apiurl: string | undefined;
  uploadDestination: string | undefined;
  fileGetUrl: string | undefined;
  redis_client: {
    host: string | undefined;
    port: number | undefined;
    password: string | undefined;
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
  jwtsecret: string;
  appname: string;
  appversion: string;
  serverport: string;
  servicepath: string;
}
export const configs: IConfig = {
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
      options: {
        encrypt: false,
      },
    } as any;
  })(),
  redis_client: {
    host: process.env.BLPOP_REDIS_INSTANCE,
    port: 6379,
    password: process.env.BLPOP_REDIS_PASSWORD,
  },
  appname: process.env.APPLICATION_NAME,
  appversion: process.env.APP_VERSION,
  servicepath: process.env.SERVICE_PATH,
  serverport: process.env.SERVER_PORT,
  jwtsecret: swagger?.jwtsecret,
  uploadDestination: swagger?.uploadDestination,
  fileGetUrl: swagger?.fileGetUrl,
  path: undefined
};

export const SWAGGER_OPTS = {
  routePrefix: swagger?.routePrefix,
  exposeRoute: true,
  swagger: {
    info: {
      title: swagger?.title,
      description: swagger?.description,
      version: swagger?.version,
    },
    servers: [
      {
        url: process.env.URLSTAGE,
        description: swagger?.descriptionStage,
      },
      {
        url: process.env.URLLOCAL,
        description: swagger?.descriptionLocal,
      },
      {
        url: process.env.URLQA,
        description: swagger?.descriptionQa,
      },
      {
        url: process.env.URLPRODUCTION,
        description: swagger.descriptionProduction,
      },
    ],
    schemes: ['http'],
    consumes: ['application/json'],
    produces: ['application/json'],
    tags: [],
    securityDefinitions: {
      apiKey: {
        description: swagger?.apiKeyDes,
        type: swagger?.type,
        name: swagger?.name,
        in: swagger?.in,
      },
    },
  },
};

export const CORS_OPTIONS = {
  origin: swagger?.origin,
  methods: swagger?.methods,
  preflightContinue: false,
  optionsSuccessStatus: 204,
  credentials: false,
  exposedHeaders: null,
  allowedHeaders: null,
  maxAge: null,
  preflight: true,
  hideOptionsRoute: true,
};

export default fp((app: FastifyInstance<Server, IncomingMessage, ServerResponse>, opts: {}, done: (err?: Error) => void) => {
  app.decorate('config', configs);
  done();
});
