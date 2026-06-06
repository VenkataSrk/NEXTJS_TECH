/**
 * @createdBy <Jayaselva M>
 * @createdOn
 */

import express from "express";
import cors from "cors";
import * as http from "http";
import { redisStore } from "./db";
import { IOServer } from "./redisadapter";
import { logger } from "./log";

export class IOPresenceServer {
    public static readonly PORT: number = 3001;
    private app: express.Application;
    private server: http.Server;
    private io: any;
    private port: string | number;

    constructor(dotenv: any) {
        dotenv.config()
        this.createApp();
        this.config();
        this.createServer();
        this.sockets();
        this.listen();
    }

    private createApp(): void {
        this.app = express();
        this.app.use(cors());
    }

    private createServer(): void {
        this.server = http.createServer(this.app);
    }

    private config(): void {
        this.port = process.env.PORT || IOPresenceServer.PORT;
    }

    private sockets(): void {
        this.io = new IOServer(this.server);
    }

    private listen(): void {

        this.server.listen(this.port, () => {
            console.log(`Presence server started...${this.port}`);
            logger.info(`Presence server started...${this.port}`);
        });

        this.app.get(`/presences/status/:domainId`, async (req, res) => {
            try {
                const domainid = req.params.domainId
                const response: any = await redisStore.hgetall(`${domainid}_presence`)
                const result = Object.values(response).map((value: any) => {
                    const i = JSON.parse(value)
                    // const details = {
                    //     status: i.statusName,
                    //     name: i.userName,
                    //     emailId: i.email,
                    //     ext: i.ext,
                    //     domainId: i.domainId
                    // }
                    return i;
                });
                res.send(result?.length > 0 ? result : `No presence details for this domain: ${domainid}`);
            } catch (err) {
                logger.error('/presences/status/:domainId', req.params.domainId, err)
                res.send({ statusCode: 500, message: 'Internal server error' });
            }
        })

        this.app.get(`/presences/status/:ext/:domainId`, async (req, res) => {
            try {
                const ext = req.params.ext
                const domainid = req.params.domainId
                const response: any = await redisStore.hmget(`${domainid}_presence`, `${domainid}_${ext}`)
                const result = Object.values(response).map((value: any) => JSON.parse(value));
                if (result?.length > 0 && result[0] !== null) {
                    // const details = {
                    //     status: result[0].statusName,
                    //     name: result[0].userName,
                    //     emailId: result[0].email,
                    //     ext: result[0].ext,
                    //     domainId: result[0].domainId
                    // }
                    res.send(result[0]);
                } else {
                    res.send(`No presence details for this ext: ${ext}`);
                }
            } catch (err) {
                logger.error('/presences/status/:ext/:domainId', req.params.ext, req.params.domainId, err)
                res.send({ statusCode: 500, message: 'Internal server error' });
            }
        })
    }

    public getApp(): express.Application {
        return this.app;
    }
}

