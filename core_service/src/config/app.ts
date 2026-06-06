/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */

import { IncomingMessage, Server, ServerResponse } from 'http';

import { config } from 'dotenv';
import { FastifyInstance } from 'fastify';
import fp from 'fastify-plugin';

const env = process.env.NODE_ENV || 'qa';
config({ path: `.env.${env}` });
export interface IConfig {
  path: any;
  apiurl: string | undefined;
  appurl: string | undefined;
  uploadDestination: string | undefined;
  fileGetUrl: string | undefined;
  paths: {
    upload_destination: string;
    fileGetUrl: string;
    upload_destination_article: string;
    articleUrl: string;
    upload_virtual_background: string;
    virtual_background_path: string;
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
  sqlMyaccountDB: {
    user: string | undefined;
    host: string | undefined;
    password: string | undefined;
    database: string | undefined;
    port: number | undefined;
    options: {
      encrypt: boolean;
    };
  };
  sqlRoleDB: {
    user: string | undefined;
    host: string | undefined;
    password: string | undefined;
    database: string | undefined;
    port: number | undefined;
    options: {
      encrypt: boolean;
    };
  };
  botSqlRoleDB: {
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
  hubspot: {
    apikey: string;
    host: string;
  };
  lorio_tele_crm_api: string;
  lorio_auth_token: string;
}

export const configs: IConfig = {
  apiurl: (() => {
    return process.env.API_URL;
  })(),
  paths: (() => {
    return {
      upload_destination: process.env.UPLOAD_DESINATION_PATH,
      fileGetUrl: process.env.FILEGETURL_PATH,
      upload_destination_article: process.env.UPLOAD_DESINATION_ARTICLE,
      articleUrl: process.env.ARTICLE_FILE_PATH,
      upload_virtual_background: process.env.UPLOAD_VIRTUAL_BACKGROUND,
      virtual_background_path: process.env.VIRTUAL_BACKGROUND_PATH
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
  sqlMyaccountDB: (() => {
    return {
      host: process.env.MYSQL_WORKTUAL_HOST,
      user: process.env.MYSQL_WORKTUAL_USER,
      password: process.env.MYSQL_WORKTUAL_PASSWORD,
      database: process.env.MYSQL_WORKTUAL_DATABASE,
      port: process.env.MYSQL_PORT,
      ssl: { rejectUnauthorized: false },
      charset: 'utf8mb4',
      insecureAuth: true,
      options: {
        encrypt: false,
      },
    } as any;
  })(),
  sqlRoleDB: (() => {
    return {
      host: process.env.MYSQL_ROLE_HOST,
      user: process.env.MYSQL_ROLE_USER,
      password: process.env.MYSQL_ROLE_PASSWORD,
      database: process.env.MYSQL_ROLE_DB,
      port: process.env.MYSQL_ROLE_PORT,
      connectionLimit: 10,
      options: {
        encrypt: false,
      },
    } as any;
  })(),
  botSqlRoleDB: (() => {
    return {
      host: process.env.MYSQL_BOT_HOST,
      user: process.env.MYSQL_BOT_USER,
      password: process.env.MYSQL_BOT_PASSWORD,
      database: process.env.MYSQL_BOT_DB,
      port: process.env.MYSQL_BOT_PORT,
      connectionLimit: 10,
      options: {
        encrypt: false,
      },
    } as any;
  })(),
  redis_client: {
    host: process.env.BLPOP_REDIS_INSTANCE,
    port: Number(process.env.SOCKET_REDIS_PORT),
    password: process.env.BLPOP_REDIS_PASSWORD,
  },
  appurl: (() => {
    return process.env.APP_URL;
  })(),
  appname: process.env.APPLICATION_NAME,
  appversion: process.env.APP_VERSION,
  servicepath: process.env.SERVICE_PATH,
  serverport: process.env.SERVER_PORT,
  jwtsecret: 'sscret',
  uploadDestination: './files',
  fileGetUrl: '/public/',
  path: undefined,
  hubspot: {
    apikey: process.env.HUBSPOT_API_KEY,
    host: process.env.HUBSPOT_HOST,
  },
  lorio_tele_crm_api: process.env.LORIO_TELE_CRM_API,
  lorio_auth_token: process.env.LORIO_AUTH_TOKEN
};

export const SWAGGER_OPTS = {
  routePrefix: 'core/docs',
  exposeRoute: true,
  swagger: {
    info: {
      title: 'core Service',
      description: ' core Service api documentation',
      version: '0.1.0',
    },
    servers: [
      {
        url: 'https://stageccaasapi.unifiedring.co.uk',
        description: 'staging',
      },
      {
        url: 'http://localhost:3001',
        description: 'development',
      },
      {
        url: 'https://ccaasapi.unifiedring.co.uk',
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
