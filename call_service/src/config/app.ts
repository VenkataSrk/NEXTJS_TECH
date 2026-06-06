/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */

import { IncomingMessage, Server, ServerResponse } from 'http';

import { config } from 'dotenv';
import { FastifyInstance } from 'fastify';
import fp from 'fastify-plugin';

const env = process.env.NODE_ENV || 'qa';
console.log(`✅ Loaded environment: ${env}`);
config({ path: `.env.${env}` });
export interface IConfig {
  paths: {
    upload_destination: string;
    upload_destination_temp: string;
    fileGetUrl: string;
  };
  chat_path: {
    upload_destination: string;
    fileGetUrl: string;
  };
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
  appname: string | undefined;
  appversion: string | undefined;
  serverport: string | undefined;
  servicepath: string | undefined;
  responseChannel: string;
}

export const configs: IConfig = {
  paths: (() => {
    return {
      upload_destination: process.env.UPLOAD_DESINATION_PATH,
      upload_destination_temp: process.env.UPLOAD_DESINATION_TEMP_PATH,
      fileGetUrl: process.env.FILEGETURL_PATH,
    };
  })(),
  chat_path: (() => {
    return {
      upload_destination: process.env.UPLOAD_DESINATION_CHAT,
      fileGetUrl: process.env.FILEGETURL_CHAT,
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
  redis_client: {
    host: process.env.BLPOP_REDIS_INSTANCE,
    port: 6379,
    password: process.env.BLPOP_REDIS_PASSWORD,
  },
  appname: process.env.APPLICATION_NAME,
  appversion: process.env.APP_VERSION,
  serverport: process.env.SERVER_PORT,
  servicepath: process.env.SERVICE_PATH,
  responseChannel: process.env.RESPONSECHANNEL,
  jwtsecret: 'sscret',
};
export const SWAGGER_OPTS = {
  routePrefix: 'call/docs',
  exposeRoute: true,
  swagger: {
    info: {
      title: 'call Service',
      description: ' call Service api documentation',
      version: '0.1.0',
    },
    servers: [
      {
        url: 'https://stageccaasapi.unifiedring.co.uk/',
        description: 'staging',
      },
      {
        url: 'http://localhost:5007',
        description: 'development',
      },
      {
        url: 'https://ccaasapi.unifiedring.co.uk/',
        description: 'production',
      },
    ],
    schemes: ['http'],
    consumes: ['application/json'],
    produces: ['application/json'],
    tags: [],
    securityDefinitions: {
      apiKey: {
        description: 'Standard Authorization header using the Key and Value scheme. Example: "key" : "authorization", "value" : "{token}"',
        type: 'apiKey',
        name: 'Authorization',
        in: 'header',
      },
    },
  },
};

export const CORS_OPTIONS = {
  origin: '*',
  methods: 'GET,HEAD,PUT,PATCH,POST,DELETE',
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
