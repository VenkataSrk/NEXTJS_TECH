/**
 * @createdBy Selvakani
 * @createdOn 27th April 2021
 */

import { createServer } from 'http';
import express from 'express';
import { IOServer } from './socketioserver';
import { logger } from './log';


export class ChatServer {
    public static readonly PORT = process.env.PORT || 8080;
    private app: express.Application = express();
    private server: any;
    private io: any;
    private port: string | number | undefined;

    constructor(dotenv: any) {
        dotenv.config();
        this.config();
        this.createServer();
        this.sockets();
        this.listen();
    }

    private createServer(): void {
        this.server = createServer(this.app);
    }

    private config(): void {
        this.port = process.env.PORT || ChatServer.PORT;
    }

    private sockets(): void {
        this.io = new IOServer(this.server);
    }

    private listen(): void {
        this.server.listen(this.port, () => {

            logger.info("Received request: ", this.port);
            console.log('Running server on port %s', this.port);
        });
    }

    public getApp(): express.Application {
        return this.app;
    }
}
