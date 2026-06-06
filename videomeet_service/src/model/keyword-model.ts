import { Schema } from "mongoose";
// import { unifiedRingMongoDB } from "../config";

const keywordschema = new Schema({
    uuid: { type: String, required: false },
    keywords: {
        type: [String],
    }

});
// module.exports = unifiedRingMongoDB.model('urr_global_keywords', keywordschema)