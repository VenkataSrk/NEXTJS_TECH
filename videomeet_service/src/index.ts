import { ChatServer } from './meeting-server';
import dotenv from 'dotenv';

let app = new ChatServer(dotenv).getApp();

export { app };