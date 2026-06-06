/**
 * @createdBy Jayaselva
 * @createdOn 18th nov 2023
 */

import { config } from 'dotenv';
import { FastifyInstance } from 'fastify';
import fp from 'fastify-plugin';
import { IncomingMessage, Server, ServerResponse } from 'http';

config();
export interface IConfig {
  appname: string;
  serverport: string;
  pushnotification: {
    ios: {
      ios_options: {
        token: {
          key: string;
          keyId: string;
          teamId: string;
        };
        proxy: {
          host: string;
          port: string;
        };
        production: boolean;
      };
    };
    android: {
      FCMkey: string;
      proxyURL: string;
    };
  };
  sqlCcaasDB: {
    user: string;
    host: string;
    password: string;
    database: string;
    port: number;
  };
}
const ENV: any = process.env;

export const configs: IConfig = {
  pushnotification: {
    ios: {
      ios_options: {
        token: {
          key: ENV.PUSHNOTIFICATION_CNF_KEY,
          keyId: ENV.PUSHNOTIFICATION_CNF_KEYID,
          teamId: ENV.PUSHNOTIFICATION_CNF_TEAM_ID,
        },
        proxy: {
          host: process.env.host,
          port: process.env.port,
        },
        production: false,
      },
    },
    android: {
      FCMkey: ENV.PUSHNOTIFICATION_FCM_KEY,
      proxyURL: `http://${process.env.PROXYHOST1}:${process.env.PROXYPORT}`,
    },
  },
  sqlCcaasDB: (() => {
    return {
      host: process.env.MYSQL_HOST,
      user: process.env.MYSQL_USER,
      password: process.env.MYSQL_PASSWORD,
      database: process.env.MYSQL_DB,
      port: process.env.MYSQL_PORT,
      insecureAuth: true,
      options: {
        encrypt: false,
      },
    } as any;
  })(),
  appname: ENV.APP_NAME,
  serverport: ENV.APP_SERVER_PORT,
};

export const SWAGGER_OPTS = {
  routePrefix: '/ccaas-call/docs',
  exposeRoute: true,
  swagger: {
    info: {
      title: 'ccaas-call',
      description: 'ccaas-call nodejs Api Service documentation',
      version: '0.1.0',
    },
    servers: [
      {
        url: ENV.APIURL,
        description: ENV.NODE_ENV,
      }
    ],
    schemes: ['http'],
    consumes: ['application/json'],
    produces: ['application/json'],
    tags: [
      {
        name: 'health',
        description: 'Health Check related end-points',
      },
      {
        name: 'ccaas-call',
        description: 'ccaas-call Check related end-points',
      },
    ],
    securityDefinitions: {
      apiKey: {
        description:
          'Standard Authorization header using the Key and Value scheme. Example: "key" : "authorization", "value" : "{token}"',
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

export default fp((app: FastifyInstance<Server, IncomingMessage, ServerResponse>, done: (err?: Error) => void) => {
  app.decorate('config', configs);

  done();
});
