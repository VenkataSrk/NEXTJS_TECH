// import { unifiedRingMongoDB } from '../config'
import { Schema } from "mongoose";

const schema = new Schema({
    uuid: { type: String },
    points: { type: Array },
});

// module.exports = unifiedRingMongoDB.model('urr_meeting_whiteboard', schema)