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
    connectTimeout?: number;
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
      connectTimeout: 10000,
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
  requestChannel: process.env.REQUESTCHANNEL,
  responseChannel: process.env.RESPONSECHANNEL,
  jwtsecret: 'sscret',
};
export default fp((app: FastifyInstance<Server, IncomingMessage, ServerResponse>, opts: {}, done: (err?: Error) => void) => {
  app.decorate('config', configs);
  done();
});
