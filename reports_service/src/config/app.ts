/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */

import { IncomingMessage, Server, ServerResponse } from 'http';

import { config } from 'dotenv';
import { FastifyInstance } from 'fastify';
import fp from 'fastify-plugin';

import fs from 'fs';
config();
export interface IConfig {
  apiurl: string | undefined;
  // mongouri: {
  //   ccaas: string | undefined;
  // };
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
  jwtsecret: string | undefined;
  appname: string | undefined;
  appversion: string | undefined;
  serverport: string | undefined;
  servicepath: string | undefined;
}

export const configs: IConfig = {
  apiurl: (() => {
    return process.env.API_URL;
  })(),
  // mongouri: (() => {
  //   return {
  //     ccaas: process.env.MONGO_CCAAS_URL,
  //   };
  // })(),
  sqlCcaasDB: (() => {
    return {
      host: process.env.MYSQL_HOST,
      user: process.env.MYSQL_USER,
      password: process.env.MYSQL_PASSWORD,
      database: process.env.MYSQL_DB,
      port: process.env.MYSQL_PORT,
      options: {
        encrypt: false,
      },
    };
  })() as any,
  appname: process.env.APPLICATION_NAME,
  appversion: process.env.APP_VERSION,
  serverport: process.env.SERVER_PORT,
  servicepath: process.env.SERVICE_PATH,
  jwtsecret: process.env.JWT_SECRET_KEY,
};
export const SWAGGER_OPTS = {
  routePrefix: 'reports/docs',
  exposeRoute: true,
  swagger: {
    info: {
      title: 'reports Service',
      description: ' reports Service api documentation',
      version: '0.1.0',
    },
    servers: [
      {
        url: 'https://stageccaasapi.unifiedring.co.uk/',
        description: 'staging',
      },
      {
        url: 'http://localhost:5002',
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
