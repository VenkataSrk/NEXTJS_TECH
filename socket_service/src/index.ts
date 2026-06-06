import dotenv from 'dotenv';
import { closeAllRedis, closeMysqlPool } from './plugins/db';
import { logger } from './plugins/log';
import { AgentQueueManager } from './socketio-redis/agentQueueManager';
import { ChatServer } from './socketio-redis/socket-server';

// Initialize queue system and cleanup corrupted data
async function initializeQueueSystem() {
  try {
    logger.info('Initializing queue system...');

    // Run emergency cleanup to clear any corrupted old queue
    await AgentQueueManager.emergencyCleanup();

    // Migrate any valid data from old queue format
    const migrated = await AgentQueueManager.migrateFromOldQueue();
    logger.info(`Queue migration complete: ${migrated} items migrated`);

    // Log initial queue health
    const metrics = await AgentQueueManager.getHealthMetrics();
    logger.info(`Queue health: ${JSON.stringify(metrics)}`);

    logger.info('Queue system initialized successfully');
  } catch (err) {
    logger.error(`Failed to initialize queue system: ${err}`);
  }
}

// Initialize queue system before starting server
initializeQueueSystem().catch((err) => {
  logger.error(`Queue initialization error: ${err}`);
});

// const socketApp = new ChatServer(dotenv).getApp();
const chatServer = new ChatServer(dotenv);
const socketApp = chatServer.getApp();
export { socketApp };

async function shutdown(): Promise<void> {
  try {
    logger.info('Shutting down server...');

    // Stop queue processing gracefully
    AgentQueueManager.stopProcessing();

    await chatServer.close();
    await closeAllRedis();
    await closeMysqlPool();

    logger.info('Server shutdown complete');
  } catch (err) {
    console.error('Error during shutdown', err);
  } finally {
    process.exit(0);
  }
}

['SIGINT', 'SIGTERM'].forEach((signal) => {
  process.on(signal as NodeJS.Signals, shutdown);
});
