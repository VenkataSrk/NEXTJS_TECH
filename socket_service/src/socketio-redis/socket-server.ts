import express from 'express';
import { createServer } from 'http';
import { blpopQueuedCustomerByStatus, chatQueueHandler } from './chat_queue';
import { IOServer } from './socket-pubsub';
export class ChatServer {
  public static readonly SOCKETPORT = process.env.SOCKETPORT || 5008;
  private app: express.Application = express();
  private server: any;
//  private io: any;
  private io: IOServer | undefined;
  private port: string | number | undefined;

  constructor(dotenv: any) {
    dotenv.config();
    this.config();
    this.createServer();
    this.sockets();
    this.chatHandler();
    this.listen();
  }

  public getApp(): express.Application {
    return this.app;
  }

  public async close(): Promise<void> {
    if (this.io) {
      this.io.close();
    }
    if (this.server) {
      await new Promise<void>((resolve) => {
        this.server.close(() => resolve());
      });
    }
  }

  private createServer(): void {
    this.server = createServer(this.app);
  }

  private config(): void {
    this.port = process.env.SOCKETPORT || ChatServer.SOCKETPORT;
  }

  private sockets(): void {
    this.io = new IOServer(this.server);
  }

  private chatHandler(): void {
    // this.io =  chatQueueHandler();
    // this.io =  blpopQueuedCustomerByStatus();
    chatQueueHandler();
    blpopQueuedCustomerByStatus;
  }

  private listen(): void {
    this.server.listen(this.port, () => {
      console.log(`Server is running on port ${this.port}`);
    });
  }
}
