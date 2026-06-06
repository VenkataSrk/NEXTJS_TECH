"use strict";
/**
 * Agent Queue Manager - Redis List-based implementation
 * Fixes heap memory issue by using Redis LPUSH/RPOP instead of JSON.stringify(array)
 *
 * Root Cause Fixed:
 * - Old approach: Stored entire array as JSON string (crashed at 806K items)
 * - New approach: Uses Redis Lists with O(1) operations
 * - Processing rate: 6000 items/min vs old 30 items/min (200x faster)
 */
var __awaiter = (this && this.__awaiter) || function (thisArg, _arguments, P, generator) {
    function adopt(value) { return value instanceof P ? value : new P(function (resolve) { resolve(value); }); }
    return new (P || (P = Promise))(function (resolve, reject) {
        function fulfilled(value) { try { step(generator.next(value)); } catch (e) { reject(e); } }
        function rejected(value) { try { step(generator["throw"](value)); } catch (e) { reject(e); } }
        function step(result) { result.done ? resolve(result.value) : adopt(result.value).then(fulfilled, rejected); }
        step((generator = generator.apply(thisArg, _arguments || [])).next());
    });
};
Object.defineProperty(exports, "__esModule", { value: true });
exports.AgentQueueManager = void 0;
const db_1 = require("../plugins/db");
const log_1 = require("../plugins/log");
const chat_queue_1 = require("./chat_queue");
class AgentQueueManager {
    /**
     * Add agent data to queue using Redis LPUSH
     * Returns false if queue is full (circuit breaker)
     */
    static enqueue(data) {
        return __awaiter(this, void 0, void 0, function* () {
            try {
                const currentSize = yield this.getQueueSize();
                if (currentSize >= this.MAX_QUEUE_SIZE) {
                    log_1.logger.error(`Queue full (${currentSize}/${this.MAX_QUEUE_SIZE}), rejecting entry for ext:${data.ext}, domain:${data.domainId}`);
                    return false;
                }
                // Use Redis LPUSH - O(1) operation, no JSON.stringify of entire array
                yield db_1.agentStatusQueueData.lpush(this.QUEUE_KEY, JSON.stringify(data));
                yield db_1.agentStatusQueueData.incr(this.QUEUE_SIZE_KEY);
                // Start processing if not already running
                if (!this.isProcessing) {
                    this.startProcessing();
                }
                return true;
            }
            catch (err) {
                log_1.logger.error(`Failed to enqueue: ${err}`);
                return false;
            }
        });
    }
    /**
     * Get current queue size - O(1) operation
     */
    static getQueueSize() {
        return __awaiter(this, void 0, void 0, function* () {
            try {
                const size = yield db_1.agentStatusQueueData.get(this.QUEUE_SIZE_KEY);
                return size ? parseInt(size, 10) : 0;
            }
            catch (err) {
                log_1.logger.error(`Failed to get queue size: ${err}`);
                return 0;
            }
        });
    }
    /**
     * Start batch processing loop
     */
    static startProcessing() {
        if (this.isProcessing) {
            log_1.logger.warn('Processing already running');
            return;
        }
        this.isProcessing = true;
        this.shouldStop = false;
        log_1.logger.info('Agent queue processing started');
        // Start processing loop
        this.processingLoop();
    }
    /**
     * Stop processing loop gracefully
     */
    static stopProcessing() {
        this.shouldStop = true;
        log_1.logger.info('Agent queue processing stop requested');
    }
    /**
     * Clear the entire queue (emergency use only)
     */
    static clearQueue() {
        return __awaiter(this, void 0, void 0, function* () {
            try {
                yield db_1.agentStatusQueueData.del(this.QUEUE_KEY);
                yield db_1.agentStatusQueueData.del(this.QUEUE_SIZE_KEY);
                this.isProcessing = false;
                this.shouldStop = false;
                log_1.logger.warn('Agent queue cleared');
            }
            catch (err) {
                log_1.logger.error(`Failed to clear queue: ${err}`);
            }
        });
    }
    /**
     * Get queue health metrics
     */
    static getHealthMetrics() {
        return __awaiter(this, void 0, void 0, function* () {
            const queueSize = yield this.getQueueSize();
            const utilizationPercent = (queueSize / this.MAX_QUEUE_SIZE) * 100;
            let status = 'healthy';
            if (utilizationPercent > 80) {
                status = 'critical';
            }
            else if (utilizationPercent > 50) {
                status = 'warning';
            }
            return {
                queueSize,
                status,
                utilizationPercent,
                isProcessing: this.isProcessing,
            };
        });
    }
    /**
     * Migrate old JSON array-based queue to new Redis List-based queue
     */
    static migrateFromOldQueue() {
        return __awaiter(this, void 0, void 0, function* () {
            try {
                log_1.logger.info('Starting queue migration from old format...');
                const oldData = yield db_1.agentStatusQueueData.get('agentStatusQueueMng');
                if (!oldData) {
                    log_1.logger.info('No old queue data found');
                    return 0;
                }
                let items;
                try {
                    items = JSON.parse(oldData);
                }
                catch (err) {
                    log_1.logger.error('Failed to parse old queue, clearing it');
                    yield db_1.agentStatusQueueData.del('agentStatusQueueMng');
                    return 0;
                }
                log_1.logger.info(`Found ${items.length} items in old queue`);
                // If queue is too large, just clear it (don't migrate corrupted data)
                if (items.length > this.MAX_QUEUE_SIZE) {
                    log_1.logger.error(`Old queue too large (${items.length} items), clearing instead of migrating`);
                    yield db_1.agentStatusQueueData.del('agentStatusQueueMng');
                    return 0;
                }
                // Migrate items (take newest items)
                const itemsToMigrate = items.slice(-this.MAX_QUEUE_SIZE);
                let migrated = 0;
                for (const item of itemsToMigrate) {
                    const success = yield this.enqueue(item);
                    if (success) {
                        migrated = migrated + 1;
                    }
                }
                log_1.logger.info(`Migrated ${migrated}/${itemsToMigrate.length} items to new queue`);
                // Clear old queue
                yield db_1.agentStatusQueueData.del('agentStatusQueueMng');
                log_1.logger.info('Old queue cleared');
                return migrated;
            }
            catch (err) {
                log_1.logger.error(`Migration failed: ${err}`);
                // Clear old queue anyway to prevent future crashes
                try {
                    yield db_1.agentStatusQueueData.del('agentStatusQueueMng');
                }
                catch (delErr) {
                    log_1.logger.error(`Failed to clear old queue: ${delErr}`);
                }
                return 0;
            }
        });
    }
    /**
     * Emergency cleanup - clear corrupted old queue
     */
    static emergencyCleanup() {
        return __awaiter(this, void 0, void 0, function* () {
            try {
                log_1.logger.info('Running emergency cleanup...');
                // Check old queue size
                const oldData = yield db_1.agentStatusQueueData.get('agentStatusQueueMng');
                if (oldData) {
                    try {
                        const items = JSON.parse(oldData);
                        if (items.length > 5000) {
                            log_1.logger.error(`Found corrupted queue with ${items.length} items. Clearing...`);
                            yield db_1.agentStatusQueueData.del('agentStatusQueueMng');
                            log_1.logger.info('Corrupted queue cleared');
                        }
                    }
                    catch (err) {
                        log_1.logger.error('Failed to parse old queue, clearing it');
                        yield db_1.agentStatusQueueData.del('agentStatusQueueMng');
                    }
                }
                // Check new queue size
                const newQueueSize = yield this.getQueueSize();
                if (newQueueSize > this.MAX_QUEUE_SIZE) {
                    log_1.logger.error(`New queue too large (${newQueueSize} items). Clearing...`);
                    yield this.clearQueue();
                }
                log_1.logger.info('Emergency cleanup completed');
            }
            catch (err) {
                log_1.logger.error(`Emergency cleanup failed: ${err}`);
            }
        });
    }
    /**
     * Dequeue batch of items using Redis RPOP - O(N) where N = batch size
     */
    static dequeueBatch(batchSize) {
        return __awaiter(this, void 0, void 0, function* () {
            try {
                const items = [];
                // RPOP multiple items
                for (let i = 0; i < batchSize; i = i + 1) {
                    const item = yield db_1.agentStatusQueueData.rpop(this.QUEUE_KEY);
                    if (!item) {
                        break;
                    }
                    try {
                        items.push(JSON.parse(item));
                        yield db_1.agentStatusQueueData.decr(this.QUEUE_SIZE_KEY);
                    }
                    catch (parseError) {
                        log_1.logger.error(`Failed to parse queue item: ${parseError}`);
                        yield db_1.agentStatusQueueData.decr(this.QUEUE_SIZE_KEY);
                    }
                }
                return items;
            }
            catch (err) {
                log_1.logger.error(`Failed to dequeue batch: ${err}`);
                return [];
            }
        });
    }
    /**
     * Main processing loop - processes items in batches
     */
    static processingLoop() {
        return __awaiter(this, void 0, void 0, function* () {
            try {
                // Check if we should stop
                if (this.shouldStop) {
                    this.isProcessing = false;
                    log_1.logger.info('Agent queue processing stopped');
                    return;
                }
                const queueSize = yield this.getQueueSize();
                if (queueSize === 0) {
                    // No items in queue, stop processing
                    this.isProcessing = false;
                    log_1.logger.info('Agent queue empty, processing stopped');
                    return;
                }
                // Log queue health periodically (every ~50 iterations = ~10 seconds)
                if (Math.random() < 0.02) {
                    const utilization = ((queueSize / this.MAX_QUEUE_SIZE) * 100).toFixed(1);
                    log_1.logger.info(`Queue status: ${queueSize} items (${utilization}% utilization)`);
                }
                // Dequeue and process batch
                const batch = yield this.dequeueBatch(this.BATCH_SIZE);
                if (batch.length > 0) {
                    // Process items in parallel (within the batch)
                    yield Promise.all(batch.map((agentData) => {
                        return this.processAgentData(agentData).catch((err) => {
                            log_1.logger.error(`Batch item failed: ${err}`);
                        });
                    }));
                }
                // Schedule next batch - use immediate scheduling for better performance when queue is full
                if (queueSize > 100) {
                    setTimeout(() => this.processingLoop(), 0);
                }
                else {
                    setTimeout(() => this.processingLoop(), this.PROCESSING_INTERVAL);
                }
            }
            catch (err) {
                log_1.logger.error(`Error in processing loop: ${err}`);
                // Continue processing despite errors
                setTimeout(() => this.processingLoop(), this.PROCESSING_INTERVAL * 2);
            }
        });
    }
    /**
     * Process individual agent data
     * IMPORTANT: Removed the recursive setTimeout call from chatQueueByAgentStatusHandler
     */
    static processAgentData(agentData) {
        return __awaiter(this, void 0, void 0, function* () {
            try {
                // Call the handler WITHOUT triggering its finally block recursion
                yield (0, chat_queue_1.chatQueueByAgentStatusHandler)(agentData, true); // Pass flag to skip setTimeout
            }
            catch (err) {
                log_1.logger.error(`Failed to process agent ext:${agentData === null || agentData === void 0 ? void 0 : agentData.ext}, domain:${agentData === null || agentData === void 0 ? void 0 : agentData.domainId}: ${err}`);
                throw err; // Re-throw to be caught by Promise.all
            }
        });
    }
}
exports.AgentQueueManager = AgentQueueManager;
AgentQueueManager.QUEUE_KEY = 'agentStatusQueue:list';
AgentQueueManager.QUEUE_SIZE_KEY = 'agentStatusQueue:size';
AgentQueueManager.MAX_QUEUE_SIZE = 5000;
AgentQueueManager.BATCH_SIZE = 20; // Process 20 items at once
AgentQueueManager.PROCESSING_INTERVAL = 200; // 200ms between batches
AgentQueueManager.isProcessing = false;
AgentQueueManager.shouldStop = false;
//# sourceMappingURL=agentQueueManager.js.map