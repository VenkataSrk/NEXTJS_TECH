// import { unifiedRingMongoDB } from '../config'
import { Schema } from "mongoose";

const schema = new Schema({
    uuid: { type: String, required: false },
    transcripts: {
        type: Array, //map
        of: new Schema({
            tid: { type: String, required: false },
            text: { type: String, required: false },
            imageURL: { type: String, required: false },
            lang: { type: String, required: false },
            timeStamp: { type: String, required: false },
            speakerName: { type: String, required: false },
            convertedText: { type: String, required: false },
            isHost: { type: String, required: false },
            deviceid: { type: String, required: false },

        }),
    },

});
