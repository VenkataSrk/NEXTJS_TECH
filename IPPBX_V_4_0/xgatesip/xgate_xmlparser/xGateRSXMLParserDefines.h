
/*
 * ******************************************************************************
 * Vectone Mobile
 * @file xGateRSParserDefines.h
 *
 * @brief This is RS Metadata node and attribute hardcode value. 
 *
 * @author Narrain prithvi Dharuman  
 * ******************************************************************************
 */
#ifndef XGATE_RS_PARSER_DEFINES
#define XGATE_RS_PARSER_DEFINES

#define RSXML_XML_NAMESPACE_TYPE              "xmlns"
#define RSXML_XML_ENCODING_TYPE               "encoding"
#define RSXML_XML_ENCODING_UTF_8              "utf-8"
#define RSXML_XML_VERSION_TYPE                "version"
#define RSXML_XML_VERSION_VALUE               "1.0"
#define RSXML_XML_FROM_NUMBER                 "from_number"
#define RSXML_XML_TO_NUMBER                   "to_number"
#define RSXML_XML_DOMAIN                      "domain_name"
#define RSXML_XML_DOMAIN_ID                   "domain_id"
#define RSXML_XML_QEXTN                       "q_extn"
#define RSXML_XML_CALLTYPE_TYPE               "calltype"
#define RSXML_RECORDING_TYPE_NODE             "recording"
#define RSXML_DATAMODE_TYPE_NODE              "datamode"
#define RSXML_CALLTYPE_TYPE_NODE              "calltype"
#define RSXML_CCAAS_RECFILENAME               "ccaas_recfile_name"
#define RSXML_CCAAS_REC_URL                   "ccaas_rec_url"
#define RSXML_CCAAS_CALL_TYPE                 "ccaas_call_type"

#define RSXML_SESSION_TYPE_NODE               "session"
#define RSXML_SESSION_TYPE_SESSIONID          "session_id"
#define RSXML_SESSION_TYPE_SIPSESSIONID       "sipSessionID"
#define RSXML_SESSION_TYPE_URCALLDATA         "urcalldata"
#define RSXML_SESSION_TYPE_CALLGROUPID        "urcallgroupid"
#define RSXML_SESSION_TYPE_URCALLDATA_VALUE   "http://unifiedrign.com/rec/recxml"

#define RSXML_PARTICIPANTS_TYPE_NODE          "participant"
#define RSXML_PARTICIPANTS_TYPE_PARTICIPANTID "participant_id"
#define RSXML_PARTICIPANTS_TYPE_NAMEID        "nameID"
#define RSXML_PARTICIPANTS_TYPE_AOR           "aor"
#define RSXML_PARTICIPANTS_TYPE_NAME          "name"
#define RSXML_PARTICIPANTS_TYPE_URCALLDATA    "urcalldata"
#define RSXML_PARTICIPANTS_TYPE_URCALLDATA_VALUE   "http://unifiedrign.com/rec/recxml"
#define RSXML_PARTICIPANTS_TYPE_CALLINGPARTY  "callingparty"
#define RSXML_PARTICIPANTS_TYPE_ODR           "odr"
#define RSXML_PARTICIPANTS_TYPE_COMMAND       "command"
#define RSXML_PARTICIPANTS_TYPE_CALLSESSIONID "callsessionId"
#define RSXML_PARTICIPANTS_TYPE_XML_LANG      "xml:lang"
#define RSXML_PARTICIPANTS_TYPE_XML_LANG_IT   "it"

#define RSXML_STREAM_TYPE_NODE                "stream"
#define RSXML_STREAM_TYPE_STREAMID            "stream_id"
#define RSXML_STREAM_TYPE_SESSIONID           "session_id"
#define RSXML_STREAM_TYPE_LABEL               "label"

#define RSXML_SESS_REC_ASSOC_TYPE_NODE        "sessionrecordassoc"
#define RSXML_SESS_REC_ASSOC_TYPE_SESSIONID   "session_id"
#define RSXML_SESS_REC_ASSOC_TYPE_ASSOC_TIME  "associate-time"

#define RSXML_PARTI_SESSION_ASSOC_TYPE_NODE   "participantsessionassoc"
#define RSXML_PARTI_SESSION_ASSOC_ID          "participant_id"
#define RSXML_PARTI_SESSION_ASSOC_SESSIONID   "session_id"
#define RSXML_PARTI_SESSION_ASSOC_TIME        "associate-time"
#define RSXML_PARTI_SESSION_DIS_ASSOC_TIME    "disassociate-time"

#define RSXML_PARTI_STREAM_ASSOC_TYPE_NODE    "participantstreamassoc"
#define RSXML_PARTI_STREAM_ASSOC_TYPE_ID      "participant_id"
#define RSXML_PARTI_STREAM_ASSOC_TYPE_SEND    "send"
#define RSXML_PARTI_STREAM_ASSOC_TYPE_RECV    "recv"

#endif 
