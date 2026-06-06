"use strict";
var __awaiter = (this && this.__awaiter) || function (thisArg, _arguments, P, generator) {
    function adopt(value) { return value instanceof P ? value : new P(function (resolve) { resolve(value); }); }
    return new (P || (P = Promise))(function (resolve, reject) {
        function fulfilled(value) { try { step(generator.next(value)); } catch (e) { reject(e); } }
        function rejected(value) { try { step(generator["throw"](value)); } catch (e) { reject(e); } }
        function step(result) { result.done ? resolve(result.value) : adopt(result.value).then(fulfilled, rejected); }
        step((generator = generator.apply(thisArg, _arguments || [])).next());
    });
};
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
exports.ChatServer = void 0;
const express_1 = __importDefault(require("express"));
const http_1 = require("http");
const chat_queue_1 = require("./chat_queue");
const socket_pubsub_1 = require("./socket-pubsub");
class ChatServer {
    constructor(dotenv) {
        this.app = (0, express_1.default)();
        dotenv.config();
        this.config();
        this.createServer();
        this.sockets();
        this.chatHandler();
        this.listen();
    }
    getApp() {
        return this.app;
    }
    close() {
        return __awaiter(this, void 0, void 0, function* () {
            if (this.io) {
                this.io.close();
            }
            if (this.server) {
                yield new Promise((resolve) => {
                    this.server.close(() => resolve());
                });
            }
        });
    }
    createServer() {
        this.server = (0, http_1.createServer)(this.app);
    }
    config() {
        this.port = process.env.SOCKETPORT || ChatServer.SOCKETPORT;
    }
    sockets() {
        this.io = new socket_pubsub_1.IOServer(this.server);
    }
    chatHandler() {
        // this.io =  chatQueueHandler();
        // this.io =  blpopQueuedCustomerByStatus();
        (0, chat_queue_1.chatQueueHandler)();
        chat_queue_1.blpopQueuedCustomerByStatus;
    }
    listen() {
        this.server.listen(this.port, () => {
            console.log(`Server is running on port ${this.port}`);
        });
    }
}
exports.ChatServer = ChatServer;
ChatServer.SOCKETPORT = process.env.SOCKETPORT || 5008;
//# sourceMappingURL=socket-server.js.map