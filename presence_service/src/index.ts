/**
 * @createdBy <Jayaselva M>
 * @createdOn
 */

import dotenv from 'dotenv';
import { IOPresenceServer } from './server';

let app = new IOPresenceServer(dotenv).getApp();
export { app };