/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */

import { IncomingMessage, Server, ServerResponse } from 'http';

import { config } from 'dotenv';
import { FastifyInstance } from 'fastify';
import fp from 'fastify-plugin';

config();
export interface IConfig {
  apiurl: string | undefined;
  paths: {
    upload_destination: string;
    upload_destination_temp: string;
    fileGetUrl: string;
  };
  chat_path: {
    upload_destination: string;
    fileGetUrl: string;
  };
  email_path: {
    Email_upload_destination: string;
    Email_fileGetUrl: string;
  };
  readSpeaker: {
    host: string;
    prefix: string;
  };
  redis_client: {
    host: string | undefined;
    port: number | undefined;
    password: string | undefined;
  };
  voiceEngineBaseUrl: string;
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
  emailService: {
    user: string | undefined;
    host: string | undefined;
    password: string | undefined;
    database: string | undefined;
    port: number | undefined;
    options: {
      encrypt: boolean;
    };
  };
  sqlMyAcccaasDB: {
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
  requestChannel: string;
  responseChannel: string;
}
const env = process.env.NODE_ENV || 'qa';
config({ path: `.env.${env}` });

export const configs: IConfig = {
  apiurl: (() => {
    return process.env.API_URL;
  })(),
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
  email_path: (() => {
    return {
      Email_upload_destination: process.env.UPLOAD_DESINATION_EMAIL,
      Email_fileGetUrl: process.env.FILEGETURL_EMAIL,
    };
  })(),
  readSpeaker: (() => {
    return {
      host: '10.22.3.70',
      prefix: '/rest/vtspeech',
    };
  })(),
  voiceEngineBaseUrl: 'http://staging.voiceengine.unifiedring.co.uk:5001/api',
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
  emailService: (() => {
    return {
      host: process.env.MYSQL_EMAIL_HOST,
      user: process.env.MYSQL_EMAIL_USER,
      password: process.env.MYSQL_EMAIL_PASSWORD,
      database: process.env.MYSQL_EMAIL_DB,
      port: process.env.MYSQL_EMAIL_PORT,
      insecureAuth: true,
      options: {
        encrypt: false,
      },
    } as any;
  })(),
  sqlMyAcccaasDB: (() => {
    return {
      host: process.env.MYSQL_ACC_HOST,
      user: process.env.MYSQL_ACC_USER,
      password: process.env.MYSQL_ACC_PASSWORD,
      database: process.env.MYSQL_ACC_DB,
      port: process.env.MYSQL_ACC_PORT,
      charset: 'utf8mb4',
      insecureAuth: true,
      options: {
        encrypt: false,
      },
    } as any;
  })(),
  redis_client: {
    host: process.env.BLPOP_REDIS_INSTANCE,
    port: Number(process.env.BLPOP_REDIS_PORT),
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
export const SWAGGER_OPTS = {
  routePrefix: 'email/docs',
  exposeRoute: true,
  swagger: {
    info: {
      title: 'email Service',
      description: ' email Service api documentation',
      version: '0.1.0',
    },
    servers: [
      {
        url: 'http://localhost:5007/',
        description: 'local',
      },
      {
        url: 'https://stageccaasapi.worktual.co.uk/',
        description: 'staging',
      },
      {
        url: 'https://qaccaasapi.worktual.co.uk/',
        description: 'qa',
      },
      {
        url: 'https://ccaasapi.worktual.co.uk/',
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
