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
exports.socketApp = void 0;
const dotenv_1 = __importDefault(require("dotenv"));
const db_1 = require("./plugins/db");
const log_1 = require("./plugins/log");
const agentQueueManager_1 = require("./socketio-redis/agentQueueManager");
const socket_server_1 = require("./socketio-redis/socket-server");
// Initialize queue system and cleanup corrupted data
function initializeQueueSystem() {
    return __awaiter(this, void 0, void 0, function* () {
        try {
            log_1.logger.info('Initializing queue system...');
            // Run emergency cleanup to clear any corrupted old queue
            yield agentQueueManager_1.AgentQueueManager.emergencyCleanup();
            // Migrate any valid data from old queue format
            const migrated = yield agentQueueManager_1.AgentQueueManager.migrateFromOldQueue();
            log_1.logger.info(`Queue migration complete: ${migrated} items migrated`);
            // Log initial queue health
            const metrics = yield agentQueueManager_1.AgentQueueManager.getHealthMetrics();
            log_1.logger.info(`Queue health: ${JSON.stringify(metrics)}`);
            log_1.logger.info('Queue system initialized successfully');
        }
        catch (err) {
            log_1.logger.error(`Failed to initialize queue system: ${err}`);
        }
    });
}
// Initialize queue system before starting server
initializeQueueSystem().catch((err) => {
    log_1.logger.error(`Queue initialization error: ${err}`);
});
// const socketApp = new ChatServer(dotenv).getApp();
const chatServer = new socket_server_1.ChatServer(dotenv_1.default);
const socketApp = chatServer.getApp();
exports.socketApp = socketApp;
function shutdown() {
    return __awaiter(this, void 0, void 0, function* () {
        try {
            log_1.logger.info('Shutting down server...');
            // Stop queue processing gracefully
            agentQueueManager_1.AgentQueueManager.stopProcessing();
            yield chatServer.close();
            yield (0, db_1.closeAllRedis)();
            yield (0, db_1.closeMysqlPool)();
            log_1.logger.info('Server shutdown complete');
        }
        catch (err) {
            console.error('Error during shutdown', err);
        }
        finally {
            process.exit(0);
        }
    });
}
['SIGINT', 'SIGTERM'].forEach((signal) => {
    process.on(signal, shutdown);
});
//# sourceMappingURL=index.js.map