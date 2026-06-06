
// import { unifiedRingMongoDB } from "../config";
import { Schema } from "mongoose";

const schema = new Schema(

    {
        hostName: { type: String },
        uuid: { type: String },
        template_id: { type: String },
        is_participant_video: { type: Boolean, required: true },
        is_use_personal_meeting_id: { type: Boolean, required: true },
        meeting_id: { type: String, required: true },
        password: { type: String },
        title: { type: String },
        personal_meeting_id: { type: String },
        msgid: { type: String },
        server_host: { type: String },
        is_cancelled: { type: Boolean },
        is_attended: { type: Boolean },
        sip_id: { type: String },
        invite_url: { type: String },
        description: { type: String },
        start_timestamp: { type: Number, required: true },
        end_timestamp: { type: Number, required: true },
        duration_minutes: { type: Number, required: true },
        timezone: { type: String },
        joining_option_type: { type: String },
        calender_type: { type: String },
        rrule: { type: String },
        is_host_myvideo: { type: Boolean, required: true },
        is_record_meeting: { type: Boolean, required: true },
        is_alternate_host: { type: Boolean, required: true },
        is_participant_join_bef_host: { type: Boolean, required: true },
        is_participant_invite_guest: { type: Boolean, required: true },
        is_participant_waiting_room: { type: Boolean, required: true },
        is_participant_mute_entry: { type: Boolean, required: true },
        is_custom_password: { type: Boolean, required: true, enum: [true, false] },
        is_without_host_continue: { type: Boolean, required: true, enum: [true, false] },
        isRecurr: { type: Boolean, required: false },
        onlyContacts: { type: Boolean, required: false },
        requireAuthentication: { type: Boolean, required: false },
        restrictScreenShare: { type: Boolean, required: false },
        recurrence: { type: String },
        ReadFlag: { type: Object },
        recurrenceId: { type: String },
        authenticationJoin: { type: String },

        is_host_myaudio: { type: Boolean, required: false },

        meetings_type: { type: Number, required: true },

        /* alternate_host: {
          type: Array,
          of: new Schema({
            sipid: { type: String },
            name: { type: String },
            ext: { type: Number },
            profile_img: { type: String },
            isHost:{type:Boolean},
            isAlternateHost:{type:Boolean}
          }),
        },
       */
        dial_in_countries: {
            type: Array,
            of: new Schema({
                country: { type: String },
                external_number: [String],
            }),
        },

        speakingLanguage: { type: String, required: false },
        files: { type: Array },

        chatSetting: {
            type: Object,
            of: new Schema({
                ischat: { type: Boolean, required: true },
                isscreenshare: { type: Boolean, required: true },
                renamethemselves: { type: Boolean, required: true },
                lockmetting: { type: Boolean, required: true },
                is_participant_unmute_themselves: { type: Boolean, required: true },
                is_participant_mute_entry: { type: Boolean, required: true },
                isWaitingroom: { type: Boolean, required: true },
                share_options: {
                    type: Object,
                    of: new Schema({
                        howmanypart: { type: String, required: true },
                        whoshare: { type: String, required: true },
                        startshare: { type: String, required: true },
                    }),
                },
            }),
        },
        participants: {
            type: Array, //map
            of: new Schema({
                sipid: { type: String, required: true },
                name: { type: String, required: false },
                ext: { type: String, required: true },
                profile_img: { type: String, required: false },
                roletype: { type: Number, required: true },
                email: { type: String, required: true },
                user_type: { type: String, required: true },
                deleteFlag: { type: String, required: false },
                is_attended: { type: Boolean, required: false, default: false },

            }),
        },
        save_template: { type: Boolean, required: true },
        status: { type: String, required: true },
        lastModifiedDate: { type: Number, required: false },
        createdat: {
            type: Date,
            default: Date.now,
        },
    }
)
export const meetingVars = {
    hostName: { type: String },
    uuid: { type: String },
    template_id: { type: String },
    is_participant_video: { type: Boolean, required: true },
    is_use_personal_meeting_id: { type: Boolean, required: true },
    meeting_id: { type: String, required: true },
    password: { type: String },
    title: { type: String },
    personal_meeting_id: { type: String },
    msgid: { type: String },
    server_host: { type: String },
    is_cancelled: { type: Boolean },
    is_attended: { type: Boolean },
    sip_id: { type: String },
    invite_url: { type: String },
    description: { type: String },
    start_timestamp: { type: Number, required: true },
    end_timestamp: { type: Number, required: true },
    duration_minutes: { type: Number, required: true },
    timezone: { type: String },
    joining_option_type: { type: String },
    calender_type: { type: String },
    rrule: { type: String },
    is_host_myvideo: { type: Boolean, required: true },
    is_record_meeting: { type: Boolean, required: true },
    is_alternate_host: { type: Boolean, required: true },
    is_participant_join_bef_host: { type: Boolean, required: true },
    is_participant_invite_guest: { type: Boolean, required: true },
    is_participant_waiting_room: { type: Boolean, required: true },
    is_participant_mute_entry: { type: Boolean, required: true },
    is_custom_password: { type: Boolean, required: true, enum: [true, false] },
    is_without_host_continue: { type: Boolean, required: true, enum: [true, false] },
    isRecurr: { type: Boolean, required: false },
    onlyContacts: { type: Boolean, required: false },
    requireAuthentication: { type: Boolean, required: false },
    restrictScreenShare: { type: Boolean, required: false },
    recurrence: { type: String },
    ReadFlag: { type: Object },
    recurrenceId: { type: String },
    authenticationJoin: { type: String },

    is_host_myaudio: { type: Boolean, required: false },

    meetings_type: { type: Number, required: true },

    /* alternate_host: {
      type: Array,
      of: new Schema({
        sipid: { type: String },
        name: { type: String },
        ext: { type: Number },
        profile_img: { type: String },
        isHost:{type:Boolean},
        isAlternateHost:{type:Boolean}
      }),
    },
   */
    dial_in_countries: {
        type: Array,
        of: new Schema({
            country: { type: String },
            external_number: [String],
        }),
    },

    speakingLanguage: { type: String, required: false },
    files: { type: Array },

    chatSetting: {
        type: Object,
        of: new Schema({
            ischat: { type: Boolean, required: true },
            isscreenshare: { type: Boolean, required: true },
            renamethemselves: { type: Boolean, required: true },
            lockmetting: { type: Boolean, required: true },
            is_participant_unmute_themselves: { type: Boolean, required: true },
            is_participant_mute_entry: { type: Boolean, required: true },
            isWaitingroom: { type: Boolean, required: true },
            share_options: {
                type: Object,
                of: new Schema({
                    howmanypart: { type: String, required: true },
                    whoshare: { type: String, required: true },
                    startshare: { type: String, required: true },
                }),
            },
        }),
    },
    participants: {
        type: Array, //map
        of: new Schema({
            sipid: { type: String, required: true },
            name: { type: String, required: false },
            ext: { type: String, required: true },
            profile_img: { type: String, required: false },
            roletype: { type: Number, required: true },
            email: { type: String, required: true },
            user_type: { type: String, required: true },
            deleteFlag: { type: String, required: false },
            is_attended: { type: Boolean, required: false, default: false },

        }),
    },
    save_template: { type: Boolean, required: true },
    status: { type: String, required: true },
    lastModifiedDate: { type: Number, required: false },
    createdat: {
        type: Date,
        default: Date.now,
    },
};

export interface IMeetings {
    uuid: string;
    template_id: string;
    hostName: string;
    is_participant_video: boolean;
    is_use_personal_meeting_id: boolean;
    meeting_id: string;
    password: string;
    personal_meeting_id: string;
    msgid: string;
    server_host: string;
    is_cancelled: boolean;
    is_attended: boolean;
    sip_id: string;
    invite_url: string;
    description: string;
    start_timestamp: number;
    end_timestamp: number;
    duration_minutes: number;
    timezone: string;
    joining_option_type: string;
    calender_type: string;
    rrule: string;
    is_host_myvideo: boolean;
    is_record_meeting: boolean;
    is_alternate_host: boolean;
    is_participant_join_bef_host: boolean;
    is_participant_invite_guest: boolean;
    is_participant_waiting_room: boolean;
    is_participant_mute_entry: boolean;
    is_without_host_continue: boolean;
    isRecurr: boolean;
    recurrence: string;
    onlyContacts: boolean;
    requireAuthentication: boolean;
    restrictScreenShare: boolean;
    is_custom_password: boolean;
    title: string;
    authenticationJoin: string;
    speakingLanguage: string;
    files: object;
    dial_in_countries: object;
    /*  alternate_host: object; */
    participants: object;
    save_template: boolean;
    status: string;
    createdat: Date | string;
    updatedat: Date | number | string;
    ReadFlag: object;
    recurrenceId: string;
    meetings_type: number;
    is_host_myaudio: boolean;
}

export interface getMeetingByID {
    uuid: string;
    template_id: string;
    hostName: string;
    is_participant_video: boolean;
    is_use_personal_meeting_id: boolean;
    meeting_id: string;
    password: string;
    personal_meeting_id: string;
    msgid: string;
    server_host: string;
    is_cancelled: boolean;
    is_attended: boolean;
    sip_id: string;
    invite_url: string;
    description: string;
    start_timestamp: number;
    end_timestamp: number;
    duration_minutes: number;
    timezone: string;
    joining_option_type: string;
    calender_type: string;
    rrule: string;
    is_host_myvideo: boolean;
    is_record_meeting: boolean;
    is_alternate_host: boolean;
    speakingLanguage: string;
    files: object;
    is_participant_join_bef_host: boolean;
    is_participant_invite_guest: boolean;
    is_participant_waiting_room: boolean;
    is_participant_mute_entry: boolean;
    is_without_host_continue: boolean;
    isRecurr: boolean;
    recurrence: string;
    authenticationJoin: string;
    recurrenceId: string;
    onlyContacts: boolean;
    requireAuthentication: boolean;
    restrictScreenShare: boolean;
    is_host_myaudio: boolean;

    meetings_type: number;

    is_custom_password: boolean;
    title: string;
    dial_in_countries: object;
    /*  alternate_host: object; */
    participants: object;
    save_template: boolean;
    status: string;
    createdat: Date | string;
    updatedat: Date | number | string;
    lastModifiedDate: number;
    ReadFlag: object;
}

// module.exports = unifiedRingMongoDB.model('urr_meetings', schema)

/* export interface IMeetingsDocument extends IMeetings, Document { }

const urr_meetings = new Schema<IMeetingsDocument>(meetingVars);

export const Meetings = unifiedRingMongoDB.model("urr_meetings", urr_meetings); */