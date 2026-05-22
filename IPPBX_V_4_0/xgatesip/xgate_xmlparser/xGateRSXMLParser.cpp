/*
 * ******************************************************************************
 * Vectone Mobile
 * @file xGateRSXMLParser.cpp
 *
 * @brief This is RS Metadata XML Parser/Encoder header file.
 *
 * @author Narrain Prithvi Dharuman
 * ******************************************************************************
 */
#include "xGateRSXMLParser.h"
#include "xGateRSXMLParserDefines.h"

xGateRSXMLParser::xGateRSXMLParser() {

}
xGateRSXMLParser::~xGateRSXMLParser() {

}

/* ==========================================================
 * This function suplies XML string to parse and create
 * XML doc.
 * =========================================================
 */
bool xGateRSXMLParser::loadParser(std::string payload) {
    std::cout << payload << std::endl;
    /* Supply XML as a string*/
    setParserString(payload);

    /* Construct a XML doc parsing the supplied String */
    return loadDocument();
}

/* =========================================================
 * This member function parse Recording Session(RS) metadata
 * ========================================================
 */
bool xGateRSXMLParser::parseXMLInfo(SipRec_RsMetadata &rsMData)
{

    std::cout << "xGateRSXMLParser::parseXMLInfo" << std::endl;
    /* Parsed XML string to data would be in RootNode
     * With The help of the root node this function traverse
     * the entire element, node, attributes to extract required
     * data.
     * Parse error would thrown by RapidXML hence missing elements
     * does not stop further traverse to collect other elements
     * may be usable.
     * */
    rapidxml::xml_node<>*root = getRootNode();
    if ( root == NULL ) return false;
    decodeRecordingInfo(root, rsMData);

    rapidxml::xml_node<>*datamode = root->first_node(RSXML_DATAMODE_TYPE_NODE);
    if ( datamode )
        decodeDataMode(datamode, rsMData);
    /* Communication GroupId not used for now */
    //decodeCSGroup(root->first_node("group")); //Communication Session Group
    #if 0
    rapidxml::xml_node<>*calltype = root->first_node(RSXML_CALLTYPE_TYPE_NODE);
    if ( calltype )
        decodeCallType(calltype, rsMData);
    #endif
    //Decode From Number from XML
    rapidxml::xml_node<>*from_number = root->first_node(RSXML_XML_FROM_NUMBER);
    if (from_number){
        decodeFromNumber(from_number, rsMData);
        rapidxml::xml_attribute<>*from_number_calltype = from_number->first_attribute();
	if(from_number_calltype){
        decodeFromCallType(from_number_calltype,rsMData);
        }
    }
    //Decode To Number from XML   
    rapidxml::xml_node<>*to_number = root->first_node(RSXML_XML_TO_NUMBER);
    if (to_number){
        decodeToNumber(to_number, rsMData);
	rapidxml::xml_attribute<>*to_number_calltype = to_number->first_attribute();
	if(to_number_calltype){
	decodeToCallType(to_number_calltype,rsMData);
	}
    }
    //Decode Domain Name from XML
    rapidxml::xml_node<>*domain_name = root->first_node(RSXML_XML_DOMAIN);
    if (domain_name)
        decodeDomainName(domain_name , rsMData);

    //Decode Domain ID from XML
    rapidxml::xml_node<>*domain_id = root->first_node(RSXML_XML_DOMAIN_ID);
    if (domain_id)
	    decodeDomainId(domain_id , rsMData);

    //Decode Call Q Extn from XML
    rapidxml::xml_node<>*q_extn = root->first_node(RSXML_XML_QEXTN);
    if (q_extn)
	decodeQExtn(q_extn , rsMData);

    //Decode Ccaas rec file name from XML
    rapidxml::xml_node<>*ccaas_recfile_name = root->first_node(RSXML_CCAAS_RECFILENAME);
    if (ccaas_recfile_name)
	decodeCcaasRecFileName(ccaas_recfile_name, rsMData);

    /* Decode Ccaas rec file url from XML */
    rapidxml::xml_node<>*ccaas_rec_url = root->first_node(RSXML_CCAAS_REC_URL);
    if (ccaas_rec_url)
	decodeCcaasRecUrl(ccaas_rec_url, rsMData);

    /* Decode Ccaas call direction from XML */
    rapidxml::xml_node<>*ccaas_call_type = root->first_node(RSXML_CCAAS_CALL_TYPE);
    if (ccaas_call_type)
	decodeCcaasCallType(ccaas_call_type, rsMData);

    for ( rapidxml::xml_node<> *session = root->first_node(RSXML_SESSION_TYPE_NODE);
        session; session = session->next_sibling(RSXML_SESSION_TYPE_NODE)) {
            SipRec_Session sessionInfo;
            decodeSession( session, sessionInfo );
            rsMData.sessionInfo.push_back(sessionInfo);
    }

    /* Traverse multiple participant details */
    for ( rapidxml::xml_node<> *parti = root->first_node(RSXML_PARTICIPANTS_TYPE_NODE);
        parti; parti = parti->next_sibling(RSXML_PARTICIPANTS_TYPE_NODE)) {
            SipRec_Participant partiInfo;
            decodeParticipants(parti, partiInfo);
            rsMData.partiInfo.push_back(partiInfo);
    }

    /* Traverse multiple stream */
    for (rapidxml::xml_node<> *stream = root->first_node(RSXML_STREAM_TYPE_NODE);
        stream; stream = stream->next_sibling(RSXML_STREAM_TYPE_NODE)) {
            SipRec_Stream streamInfo;
            decodeStreams( stream, streamInfo );
            rsMData.streamInfo.push_back(streamInfo);
    }

    /* sessionrecordassoc associate_time */
    for (rapidxml::xml_node<> *sess_rec_assoc = root->first_node(RSXML_SESS_REC_ASSOC_TYPE_NODE);
        sess_rec_assoc; sess_rec_assoc = sess_rec_assoc->next_sibling(
        RSXML_SESS_REC_ASSOC_TYPE_NODE)) {
            SipRec_SessionRecAssoc  sessRecAssocInfo;
            decodeSessionRecordAssoc( sess_rec_assoc, sessRecAssocInfo);
            rsMData.sessRecAssocInfo.push_back(sessRecAssocInfo);
    }

    /* extracting participantsessionassoc */
    for (rapidxml::xml_node<> *parti_sess_assoc = root->
        first_node(RSXML_PARTI_SESSION_ASSOC_TYPE_NODE);
        parti_sess_assoc; parti_sess_assoc = parti_sess_assoc->
        next_sibling(RSXML_PARTI_SESSION_ASSOC_TYPE_NODE)) {
            SipRec_PartiSessionAssoc partiSessAssocInfo;
            decodePartiSessionAssoc( parti_sess_assoc, partiSessAssocInfo );
            rsMData.partiSessAssocInfo.push_back(partiSessAssocInfo);
    }

    /* extracting participantstreamassoc*/
    for (rapidxml::xml_node<> *parti_stream_assoc = root->first_node(RSXML_PARTI_STREAM_ASSOC_TYPE_NODE);
        parti_stream_assoc; parti_stream_assoc = parti_stream_assoc->
          next_sibling("participantstreamassoc")) {
            SipRec_PartiStreamAssoc partiStreamAssocInfo;
            decodePartiStreamAssoc( parti_stream_assoc, partiStreamAssocInfo );
            rsMData.partiStreamAssocInfo.push_back(partiStreamAssocInfo);
    }
    return true;
}

/* =========================================================
 * ========================================================
 */
void xGateRSXMLParser::serializeXMLInfo(){

}

/* =========================================================
 * Decode and Check whether the Parsed XML contains "recording"
 * namespace application has to verified there should be only one
 * recording namespace in the XML body
 * ========================================================
 */
bool xGateRSXMLParser::decodeRecordingInfo( rapidxml::xml_node<>*node,
        SipRec_RsMetadata &rsMData )
{

    std::string recordingNS;
    if ( !getAttributeValue(RSXML_XML_NAMESPACE_TYPE, node, recordingNS))
        return false;

    rsMData.recordingNS     = recordingNS;
    std::cout << "xGateRSXMLParser::decodeRecordingInfo: "
              << recordingNS << std::endl;
    return  true;
}

/* =========================================================
 * Decode to check datamode type either complete or partial.
 * non exist of datamode should be 'complete' type.
 * ========================================================
 */
bool xGateRSXMLParser::decodeDataMode( rapidxml::xml_node<>*node,
        SipRec_RsMetadata &rsMData ) {
    std::string dataMode = node->value();
    rsMData.dataMode     = dataMode;
    std::cout << "xGateRSXMLParser::DataMode: " << dataMode  << std::endl;
    std::cout << std::endl;
    return true;
}

/* =========================================================
 * Decode to check calltype type either complete or partial.
 * non exist of calltype should be 'complete' type.
 * ========================================================
 */
bool xGateRSXMLParser::decodeCallType( rapidxml::xml_node<>*node,
        SipRec_RsMetadata &rsMData ) {
    std::string callType = node->value();
    rsMData.callType     = callType;
    std::cout << "xGateRSXMLParser::CallType: " << callType  << std::endl;
    std::cout << std::endl;
    return true;
}

/*===================================================
 */
bool xGateRSXMLParser::decodeFromNumber( rapidxml::xml_node<>*node,
        SipRec_RsMetadata &rsMData ) {
    std::string from_number = node->value();
    rsMData.from_number     = from_number;
    //rsMData.from_calltype   = node->m_first_attribute->m_value;
    std::cout << "xGateRSXMLParser::from_number: " << from_number  << std::endl;
    std::cout << std::endl;
    return true;
}

/*====================================================
 */
bool xGateRSXMLParser::decodeFromCallType(rapidxml::xml_attribute<>*attribute,
	SipRec_RsMetadata& rsMData){
     std::string calltype = attribute->value();
     rsMData.from_calltype = calltype;
     std::cout<<"xGateRSXMLParser::decodeFromCallType: " << calltype << std::endl;
     return true;
}

/*===================================================
 */
bool xGateRSXMLParser::decodeToNumber( rapidxml::xml_node<>*node,
        SipRec_RsMetadata &rsMData ) {
    std::string to_number = node->value();
    rsMData.to_number     = to_number;
    //rsMData.to_calltype   = node->m_first_attribute->m_value;
    std::cout << "xGateRSXMLParser::to_number: " << to_number  << std::endl;
    std::cout << std::endl;
    return true;
}

/*===================================================
 */

bool xGateRSXMLParser::decodeToCallType(rapidxml::xml_attribute<>*attribute,
        SipRec_RsMetadata& rsMData){
     std::string calltype = attribute->value();
     rsMData.to_calltype = calltype;
     std::cout<<"xGateRSXMLParser::decodeFromCallType: " << calltype << std::endl;
     return true;
}

/*===================================================
 */
bool xGateRSXMLParser::decodeDomainName( rapidxml::xml_node<>*node,
        SipRec_RsMetadata &rsMData ) {
    std::string domain_name  = node->value();
    rsMData.domain_name      = domain_name;
    std::cout << "xGateRSXMLParser::domain_name " << domain_name  << std::endl;
    std::cout << std::endl;
    return true;
}

/*===================================================
 */
bool xGateRSXMLParser::decodeDomainId( rapidxml::xml_node<>*node,
        SipRec_RsMetadata &rsMData ) {
    std::string domain_id  = node->value();
    rsMData.domain_id      = domain_id;
    std::cout << "xGateRSXMLParser::domain_id " << domain_id  << std::endl;
    std::cout << std::endl;
    return true;
}

/*===================================================
 */

bool xGateRSXMLParser::decodeQExtn( rapidxml::xml_node<>*node,
        SipRec_RsMetadata &rsMData ) {
    std::string qExtn = node->value();
    rsMData.q_extn    = qExtn;
    std::cout << "xGateRSXMLParser::QExtn: " << qExtn  << std::endl;
    std::cout << std::endl;
    return true;
}

bool xGateRSXMLParser::decodeCcaasRecFileName(rapidxml::xml_node<>*node, SipRec_RsMetadata &rsMData)
{
    std::string ccaasRecFileName = node->value();
    rsMData.ccaas_recfile_name   = ccaasRecFileName;
    std::cout << "xGateRSXMLParser::QExtn: " << ccaasRecFileName  << std::endl;
    std::cout << std::endl;
    return true;
}

bool xGateRSXMLParser::decodeCcaasRecUrl(rapidxml::xml_node<>*node, SipRec_RsMetadata &rsMData)
{
    std::string ccaasRecUrl = node->value();
    rsMData.ccaas_rec_url   = ccaasRecUrl;
    std::cout << "xGateRSXMLParser::decodeCcaasRecUrl: " << ccaasRecUrl  << std::endl;
    std::cout << std::endl;
    return true;
}

bool xGateRSXMLParser::decodeCcaasCallType(rapidxml::xml_node<>*node, SipRec_RsMetadata &rsMData)
{
    std::string ccaasCallType = node->value();
    rsMData.ccaas_call_type   = ccaasCallType;
    std::cout << "xGateRSXMLParser::decodeCcaasCallType: " << ccaasCallType  << std::endl;
    std::cout << std::endl;
    return true;
}

/* =========================================================
 * Decode Session will help to parse session information
 * The Communication Sessoion linked with participants and
 * media stream would be decoded by respecting member functions.
 * Please note Communication Session would be part of one CS Group.
 *
 * ========================================================
 */
bool xGateRSXMLParser::decodeSession( rapidxml::xml_node<>*node,
        SipRec_Session& sessionInfo)
{
    std::cout << "xGateRSXMLParser::decodeSession" << std::endl;

    std::string sessionID;
    if ( !getAttributeValue(RSXML_SESSION_TYPE_SESSIONID, node, sessionID))
        return false;

    sessionInfo.sessionID = sessionID;
    std::cout << "Session::SessionId: " << sessionID << std::endl;

    rapidxml::xml_node<> *SIPSessionID = node->first_node(RSXML_SESSION_TYPE_SIPSESSIONID);
    if (node == NULL) return false;
    std::string sipSessionID  = SIPSessionID->value();
    sessionInfo.sipSessionID = sipSessionID;
    std::cout << "Session::SIPSessionId: " << sipSessionID << std::endl;

    rapidxml::xml_node<> *urcalldata  = node->first_node(RSXML_SESSION_TYPE_URCALLDATA);
    if ( urcalldata == NULL ) return false;

    rapidxml::xml_node<> *callgroupid = urcalldata->first_node(RSXML_SESSION_TYPE_CALLGROUPID);
    if( callgroupid ) {
        std::string groupRef = callgroupid->value();
        sessionInfo.groupRef = groupRef;
        std::cout << "Session::callgroupid" << groupRef << std::endl;
    }
    std::cout << std::endl;
    return true;

}

/* =========================================================
 * This function decodes participant details. mainly participant
 * name and nameID(AOR-Addr or Record)
 * One session may have multiple participant with same SessionID
 *
 * ========================================================
 */
bool xGateRSXMLParser::decodeParticipants( rapidxml::xml_node<>*node,
        SipRec_Participant& partiInfo )
{
    std::cout << "xGateRSXMLParser::decodeParticipants" << std::endl;

    std::string parti_id;
    if ( !getAttributeValue(RSXML_PARTICIPANTS_TYPE_PARTICIPANTID, node, parti_id) )
        return false;

    partiInfo.participantID = parti_id;
    std::cout << "Participant::participant_id: " << parti_id << std::endl;

    rapidxml::xml_node<> *nameID = node->first_node(RSXML_PARTICIPANTS_TYPE_NAMEID);
    if (!nameID) {
        std::cout << "Error: Participant incomplete.....!" << std::endl;
    }

    std::string aor_value;
    if ( !getAttributeValue(RSXML_PARTICIPANTS_TYPE_AOR, nameID, aor_value) )
        return false;
    partiInfo.nameID      = aor_value;
    std::cout << "Participant::AddressOfRecord: " << aor_value << std::endl;

    rapidxml::xml_node<> *Name = nameID->first_node(RSXML_PARTICIPANTS_TYPE_NAMEID);

    if (Name == NULL) return false;

    std::string name_value = Name->value();
    partiInfo.name         = name_value;
    std::cout << "Participant::Name: " << name_value << std::endl;

    /*
     * urcalldata
     */
    rapidxml::xml_node<> *urcalldata  = nameID->next_sibling(RSXML_PARTICIPANTS_TYPE_URCALLDATA);
    if (urcalldata == NULL ) return false;
    /* Parse sting does not contains second pari odr command and callsessionid*/

    rapidxml::xml_node<> *callingparty = urcalldata->first_node(RSXML_PARTICIPANTS_TYPE_CALLINGPARTY);
    if(callingparty) {
        std::string callingparty_value = callingparty->value();
        partiInfo.callingParty         = callingparty_value;
        std::cout << "Participant::callingparty value: " << callingparty_value<< std::endl;
    }

    rapidxml::xml_node<> *odr         = urcalldata->first_node(RSXML_PARTICIPANTS_TYPE_ODR);
    if (odr) {
 
        std::string command;
        if ( !getAttributeValue(RSXML_PARTICIPANTS_TYPE_COMMAND, odr, command) )
            return false;
        partiInfo.odr       = command;
        std::cout << "Participant::Odr_Command: " << command << std::endl;
    }

    rapidxml::xml_node<> *callSessionID = urcalldata->first_node(RSXML_PARTICIPANTS_TYPE_CALLSESSIONID);
    if(callSessionID) {
        std::string callSessionID_value = callSessionID->value();
        partiInfo.callSessionID         = callSessionID_value;
        std::cout << "Participant::callSessionID_value: " << callSessionID_value<< std::endl;
    }
    std::cout << std::endl;
    return true;
}

/* =========================================================
 * Decoding Media metadata 'Label' from SDP or from RS Metada.
 *
 * ========================================================
 */
bool xGateRSXMLParser::decodeStreams( rapidxml::xml_node<>*node,
        SipRec_Stream& streamInfo )
{
    std::cout << "xGateRSXMLParser::decodeStreams" << std::endl;

    std::string stream_id;
    if ( !getAttributeValue(RSXML_STREAM_TYPE_STREAMID, node, stream_id) )
        return false;

    streamInfo.streamID = stream_id;
    std::cout << "Stream::stream_id: " << stream_id << std::endl;

    std::string session_id;
    if ( !getAttributeValue(RSXML_STREAM_TYPE_SESSIONID, node, session_id) )
        return false;
    streamInfo.sessionID = session_id;
    std::cout << "Stream::session_id: " << session_id << std::endl;

    rapidxml::xml_node<> *Label = node->first_node(RSXML_STREAM_TYPE_LABEL);
    if(Label) {
        std::string label_value = Label->value();
        streamInfo.label = label_value;
        std::cout << "Stream::Label: " << label_value << std::endl;
    }
    std::cout << std::endl;
    return true;
}

/* =========================================================
 * Decoding Session Rec Association with SessionId and assotiate
 * time is extracted by this function.
 * ========================================================
 */
bool xGateRSXMLParser::decodeSessionRecordAssoc( rapidxml::xml_node<>*node,
        SipRec_SessionRecAssoc& sessRecAssocInfo)
{
    std::cout << "xGateRSXMLParser::decodeSessionRecordAssoc" << std::endl;

    std::string session_id;
    if ( !getAttributeValue(RSXML_SESS_REC_ASSOC_TYPE_SESSIONID, node, session_id ) )
                    return false;
    sessRecAssocInfo.sessionID = session_id;
    std::cout << "SessionRecordAssoc::SessionID: " << session_id << std::endl;

    rapidxml::xml_node<> *associate_time = node->first_node(RSXML_SESS_REC_ASSOC_TYPE_ASSOC_TIME);
    if(associate_time) {
        std::string associate_value = associate_time->value();
        sessRecAssocInfo.associateTime  = associate_value;
        std::cout << "SessionRecordAssoc::Associate_Time: " << associate_value << std::endl;
    }
    std::cout << std::endl;
    return true;
}


/* =========================================================
 * This function extracts association btw participant, sessoion
 * and assiciated time.
 * ========================================================
 */
bool xGateRSXMLParser::decodePartiSessionAssoc(rapidxml::xml_node<>*node,
        SipRec_PartiSessionAssoc& partiSessAssocInfo)
{
    std::cout << "xGateRSXMLParser::decodePartiSessionAssoc" << std::endl;

    std::string participant_id;
    if ( !getAttributeValue(RSXML_PARTI_SESSION_ASSOC_ID, node, participant_id) )
        return false;

    partiSessAssocInfo.participantID = participant_id;
    std::cout << "PartiSessionAssoc::ParticipantID: " << participant_id << std::endl;

    std::string session_id;
    if ( !getAttributeValue(RSXML_PARTI_SESSION_ASSOC_SESSIONID, node, session_id))
        return false;

    partiSessAssocInfo.sessionID     = session_id;
    std::cout << "PartiSessionAssoc::SessionID: " << session_id << std::endl;

    rapidxml::xml_node<> *associate_time = node->first_node(RSXML_PARTI_SESSION_ASSOC_TIME);
    if (associate_time) {
        std::string associate_value = associate_time->value();
        partiSessAssocInfo.associateTime = associate_value;
        std::cout << "PartiSessionAssoc::AssociateTime: " << associate_value << std::endl;
    }

    rapidxml::xml_node<> *disassociate_time = node->first_node(RSXML_PARTI_SESSION_DIS_ASSOC_TIME);
    if (disassociate_time) {
        std::string disassociate_value = disassociate_time->value();
        partiSessAssocInfo.disAssociateTime = disassociate_value;
        std::cout << "PartiSessionAssoc::DisAssociateTime: " << disassociate_value << std::endl;
    }
    std::cout << std::endl;
    return true;
}

/* =========================================================
 * This Function extracts Participant and Stream association
 * ========================================================
 */
bool xGateRSXMLParser::decodePartiStreamAssoc(rapidxml::xml_node<>*node,
        SipRec_PartiStreamAssoc& partiStreamAssocInfo)
{
    std::cout << "xGateRSXMLParser::decodePartiStreamAssoc" << std::endl;

    std::string participantID;
    if ( !getAttributeValue(RSXML_PARTI_STREAM_ASSOC_TYPE_ID, node, participantID) )
        return false;
    partiStreamAssocInfo.participantID   = participantID;
    std::cout << "PartiStreamAssoc::SessionID: " << participantID << std::endl;

    rapidxml::xml_node<> *send= node->first_node(RSXML_PARTI_STREAM_ASSOC_TYPE_SEND);
    if (send) {
        std::string send_value = send->value();
        partiStreamAssocInfo.send        = send_value;
        std::cout << "PartiStreamAssoc::Send: " << send_value << std::endl;
    }

    rapidxml::xml_node<> *recv = node->first_node(RSXML_PARTI_STREAM_ASSOC_TYPE_RECV);
    if (recv) {
        std::string recv_value = recv->value();
        partiStreamAssocInfo.recv        = recv_value;
        std::cout << "PartiStreamAssoc::Recv: " << recv_value << std::endl;
    }
    std::cout << std::endl;
    return true;
}

/*
 * This funtion takes RS Metada to to construct XML.
 */
void xGateRSXMLParser::encodeRSMetadata( SipRec_RsMetadata rsMData )
{
    rapidxml::xml_document<> doc;
    std::cout << "xGateRSXMLParser::encodeRSMetadata" << std::endl;

    // xml declaration
    rapidxml::xml_node<>* decl;
    try {
        decl = doc.allocate_node(rapidxml::node_declaration);
    } catch (const std::exception& e) {
            std::cout << "" << e.what() << std::endl;
    }

    decl->append_attribute(doc.allocate_attribute(RSXML_XML_VERSION_TYPE,
                            RSXML_XML_VERSION_VALUE));
    decl->append_attribute(doc.allocate_attribute(RSXML_XML_ENCODING_TYPE,
                            RSXML_XML_ENCODING_UTF_8));
    doc.append_node(decl);
    
    // root node
    rapidxml::xml_node<>* root;
    try {
        root = doc.allocate_node(rapidxml::node_element,
        RSXML_RECORDING_TYPE_NODE);
    } catch (const std::exception& e) {
        std::cout << "" << e.what() << std::endl;
    }

    root->append_attribute(doc.allocate_attribute(RSXML_XML_NAMESPACE_TYPE,
        rsMData.recordingNS.c_str()));
    doc.append_node(root);

    rapidxml::xml_node<>* datamode_node;
    try {
        datamode_node = doc.allocate_node(rapidxml::node_element,
            RSXML_DATAMODE_TYPE_NODE, rsMData.dataMode.c_str());
    } catch (const std::exception& e) {
        std::cout << "" << e.what() << std::endl;
    }

    root->append_node(datamode_node);

    rapidxml::xml_node<>* calltype_node;
    try {
        calltype_node = doc.allocate_node(rapidxml::node_element,
            RSXML_CALLTYPE_TYPE_NODE, rsMData.callType.c_str());
    } catch (const std::exception& e) {
        std::cout << "" << e.what() << std::endl;
    }

    root->append_node(calltype_node);

    for (std::vector<SipRec_Session>::iterator i = rsMData.sessionInfo.begin();
        i != rsMData.sessionInfo.end(); ++i ) {
            encodeSessionInfo(doc, root, *i);
    }

    /*Participant details*/
    for (std::vector<SipRec_Participant>::iterator i = rsMData.partiInfo.begin();
        i != rsMData.partiInfo.end(); ++i ) {
            encodeParticipantInfo(doc, root, *i);
    }

    /*Stream to XML*/
    for (std::vector<SipRec_Stream>::iterator i = rsMData.streamInfo.begin();
        i != rsMData.streamInfo.end(); ++i ) {
            encodeStreamInfo(doc, root, *i);
    }

    /*sessionrecordingassoc to XML*/
    for (std::vector<SipRec_SessionRecAssoc>::iterator i = rsMData.sessRecAssocInfo.begin();
        i != rsMData.sessRecAssocInfo.end(); ++i ) {
            encodeSessionRecordAssoc(doc, root, *i);
    }

    /*participantsessionassoc to XML*/
    for (std::vector<SipRec_PartiSessionAssoc>::iterator i = rsMData.partiSessAssocInfo.begin();
        i != rsMData.partiSessAssocInfo.end(); ++i ) {
            encodePartiSessionAssoc(doc, root, *i);
    }

    /*participantstreamassoc to XML*/
    for (std::vector<SipRec_PartiStreamAssoc>::iterator i = rsMData.partiStreamAssocInfo.begin();
        i != rsMData.partiStreamAssocInfo.end(); ++i ) {
            encodePartiStreamAssoc(doc, root, *i);
    }

    std::string toXMLString;
    print(std::back_inserter(toXMLString), doc);//, rapidxml::print_no_indenting);
    std::cout << "The Encoded XML " << std::endl;
    std::cout << toXMLString;

}

/*
 * This funtion encodes session information into XML string
 */ 
void xGateRSXMLParser::encodeSessionInfo(rapidxml::xml_document<>& doc, rapidxml::xml_node<> *node,
                      SipRec_Session &sessionInfo)
{
    std::cout << "xGateRSXMLParser::encodeSessionInfo" << std::endl;

    rapidxml::xml_node<>* session_node;
    try{
        session_node = doc.allocate_node(rapidxml::node_element,
                RSXML_SESSION_TYPE_NODE);
    } catch (const std::exception& e) {
        std::cout << "" << e.what() << std::endl;
    }

    session_node->append_attribute(doc.allocate_attribute(RSXML_SESSION_TYPE_SESSIONID,
                    sessionInfo.sessionID.c_str()));
    node->append_node(session_node);

    rapidxml::xml_node<>* sipsession_node;
    try {
       sipsession_node  = doc.allocate_node(rapidxml::node_element,
                    RSXML_SESSION_TYPE_SIPSESSIONID, sessionInfo.sipSessionID.c_str());
    } catch (const std::exception& e) {
        std::cout << "" << e.what() << std::endl;
    }
    session_node->append_node(sipsession_node);

    /*urcalldata is added inside groupid */
    rapidxml::xml_node<>* urcalldata;
    try {
        urcalldata = doc.allocate_node(rapidxml::node_element,
                    RSXML_SESSION_TYPE_URCALLDATA);
    } catch (const std::exception& e) {
        std::cout << "" << e.what() << std::endl;
    }
    urcalldata->append_attribute(doc.allocate_attribute(RSXML_XML_NAMESPACE_TYPE,
                    RSXML_SESSION_TYPE_URCALLDATA_VALUE));
    rapidxml::xml_node<>* callgroupid_node;
    try {
        callgroupid_node = doc.allocate_node(rapidxml::node_element,
                    RSXML_SESSION_TYPE_CALLGROUPID, sessionInfo.groupRef.c_str());
    } catch (const std::exception& e) {
        std::cout << "" << e.what() << std::endl;
    }
    urcalldata->append_node(callgroupid_node);
    session_node->append_node(urcalldata);
}

/*
 * This function encodes participant details into XML string.
 */ 
void xGateRSXMLParser::encodeParticipantInfo(rapidxml::xml_document<>& doc,
         rapidxml::xml_node<> *node, SipRec_Participant &partiInfo)
{
    std::cout<< "xGateRSXMLParser::encodeParticipantInfo" << std::endl;
    rapidxml::xml_node<>* parti_node;
    try {
        parti_node = doc.allocate_node(rapidxml::node_element, RSXML_PARTICIPANTS_TYPE_NODE);
    } catch (const std::exception& e) {
        std::cout << "" << e.what() << std::endl;
    }
    parti_node->append_attribute(doc.allocate_attribute(RSXML_PARTICIPANTS_TYPE_PARTICIPANTID,
         partiInfo.participantID.c_str()));
    node->append_node(parti_node);

    rapidxml::xml_node<>* nameid_node;
    try {
         nameid_node = doc.allocate_node(rapidxml::node_element,
         RSXML_PARTICIPANTS_TYPE_NAMEID);
    } catch (const std::exception& e) {
        std::cout << "" << e.what() << std::endl;
    }
    nameid_node->append_attribute(doc.allocate_attribute(RSXML_PARTICIPANTS_TYPE_AOR,
                    partiInfo.nameID.c_str()));
    parti_node->append_node(nameid_node);

    rapidxml::xml_node<>* name_node;
    try {
        name_node = doc.allocate_node(rapidxml::node_element,
             RSXML_PARTICIPANTS_TYPE_NAME, partiInfo.name.c_str());
    } catch (const std::exception& e) {
        std::cout << "" << e.what() << std::endl;
    }
    name_node->append_attribute(doc.allocate_attribute(
         RSXML_PARTICIPANTS_TYPE_XML_LANG, RSXML_PARTICIPANTS_TYPE_XML_LANG_IT));
    nameid_node->append_node(name_node);

    /*urcalldata for participant*/
    rapidxml::xml_node<>* p_urcalldata;
    try {
        p_urcalldata = doc.allocate_node(rapidxml::node_element,
             RSXML_PARTICIPANTS_TYPE_URCALLDATA);
    } catch (const std::exception& e) {
        std::cout << "" << e.what() << std::endl;
    }
    p_urcalldata->append_attribute(doc.allocate_attribute(RSXML_XML_NAMESPACE_TYPE,
         RSXML_PARTICIPANTS_TYPE_URCALLDATA_VALUE));
    parti_node->append_node(p_urcalldata);

    rapidxml::xml_node<>* callingparty_node;
    try {
        callingparty_node = doc.allocate_node(rapidxml::node_element,
             RSXML_PARTICIPANTS_TYPE_CALLINGPARTY, partiInfo.callingParty.c_str());
    } catch (const std::exception& e) {
        std::cout << "" << e.what() << std::endl;
    }
    p_urcalldata->append_node(callingparty_node);

    rapidxml::xml_node<>* odr_node;
    try {
        odr_node = doc.allocate_node(rapidxml::node_element,
             RSXML_PARTICIPANTS_TYPE_ODR);
    } catch (const std::exception& e) {
        std::cout << "" << e.what() << std::endl;
    }
    odr_node->append_attribute(doc.allocate_attribute(RSXML_PARTICIPANTS_TYPE_COMMAND,
                    partiInfo.odr.c_str()));
    p_urcalldata->append_node(odr_node);

    rapidxml::xml_node<>* callsession_node;
    try {
        callsession_node = doc.allocate_node(rapidxml::node_element,
            RSXML_PARTICIPANTS_TYPE_CALLSESSIONID, partiInfo.callSessionID.c_str());
    } catch (const std::exception& e) {
        std::cout << "" << e.what() << std::endl;
    }
    p_urcalldata->append_node(callsession_node);

}

/*
 * This Function encodes stream information into XML.
 */ 
void xGateRSXMLParser::encodeStreamInfo(rapidxml::xml_document<>& doc, rapidxml::xml_node<> *node,
                      SipRec_Stream &streamInfo)
{
    std::cout << "xGateRSXMLParser::encodeStreamInfo" << std::endl;
    rapidxml::xml_node<>* stream_node;
    try {
        stream_node = doc.allocate_node(rapidxml::node_element, RSXML_STREAM_TYPE_NODE);
    } catch (const std::exception& e) {
        std::cout << "" << e.what() << std::endl;
    }
    stream_node->append_attribute(doc.allocate_attribute(RSXML_STREAM_TYPE_STREAMID,
                    streamInfo.streamID.c_str()));
    stream_node->append_attribute(doc.allocate_attribute(RSXML_STREAM_TYPE_SESSIONID,
                    streamInfo.sessionID.c_str()));
    node->append_node(stream_node);
    rapidxml::xml_node<>* label_node;
    try {
        label_node = doc.allocate_node(rapidxml::node_element,
                    RSXML_STREAM_TYPE_LABEL, streamInfo.label.c_str());
    } catch (const std::exception& e) {
        std::cout << "" << e.what() << std::endl;
    }
    stream_node->append_node(label_node);

}

/*
 * This function converts Session Record Association infor in XML.
 */ 
void xGateRSXMLParser::encodeSessionRecordAssoc(rapidxml::xml_document<>& doc, rapidxml::xml_node<> *node,
                      SipRec_SessionRecAssoc & sessRecAssocInfo)
{
    std::cout << "xGateRSXMLParser::encodeSessionRecordAssoc" << std::endl;
    rapidxml::xml_node<>* ses_rec_asso_node;
    try {
        ses_rec_asso_node = doc.allocate_node(rapidxml::node_element, RSXML_SESS_REC_ASSOC_TYPE_NODE);
    } catch (const std::exception& e) {
        std::cout << "" << e.what() << std::endl;
    }
    ses_rec_asso_node->append_attribute(doc.allocate_attribute(RSXML_SESS_REC_ASSOC_TYPE_SESSIONID,
                    sessRecAssocInfo.sessionID.c_str()));
    node->append_node(ses_rec_asso_node);
    rapidxml::xml_node<>* associate_time_node;
    try {
        associate_time_node = doc.allocate_node(rapidxml::node_element,
            RSXML_SESS_REC_ASSOC_TYPE_ASSOC_TIME, sessRecAssocInfo.associateTime.c_str());
    } catch (const std::exception& e) {
        std::cout << "" << e.what() << std::endl;
    }
    ses_rec_asso_node->append_node(associate_time_node);

}

/*
 * This function converts Participant Session Association information into XML String.
 */ 
void xGateRSXMLParser::encodePartiSessionAssoc(rapidxml::xml_document<>& doc, rapidxml::xml_node<> *node,
                      SipRec_PartiSessionAssoc &partiSessAssocInfo)
{
    std::cout << "xGateRSXMLParser::encodePartiSessionAssoc" << std::endl;

    rapidxml::xml_node<>* parti_ses_asso_node;
    try {
        parti_ses_asso_node = doc.allocate_node(rapidxml::node_element,
            RSXML_PARTI_SESSION_ASSOC_TYPE_NODE);
    } catch (const std::exception& e) {
        std::cout << "" << e.what() << std::endl;
    }
    parti_ses_asso_node->append_attribute(doc.allocate_attribute(RSXML_PARTI_SESSION_ASSOC_ID,
        partiSessAssocInfo.participantID.c_str()));
    parti_ses_asso_node->append_attribute(doc.allocate_attribute(
        RSXML_PARTI_SESSION_ASSOC_SESSIONID, partiSessAssocInfo.sessionID.c_str()));
    node->append_node(parti_ses_asso_node);
    rapidxml::xml_node<>* assoc_time_node ;
    try {
       assoc_time_node = doc.allocate_node(rapidxml::node_element,
            RSXML_PARTI_SESSION_ASSOC_TIME, partiSessAssocInfo.associateTime.c_str());
    } catch (const std::exception& e) {
        std::cout << "" << e.what() << std::endl;
    }
    parti_ses_asso_node->append_node(assoc_time_node);
    rapidxml::xml_node<>* disassoc_time_node ;
    try {
       disassoc_time_node = doc.allocate_node(rapidxml::node_element,
            RSXML_PARTI_SESSION_DIS_ASSOC_TIME, partiSessAssocInfo.disAssociateTime.c_str());
    } catch (const std::exception& e) {
        std::cout << "" << e.what() << std::endl;
    }
    parti_ses_asso_node->append_node(disassoc_time_node);

}

/*
 * This function converts Participant Stream Association information into XML String.
 */ 
void xGateRSXMLParser::encodePartiStreamAssoc(rapidxml::xml_document<>& doc, rapidxml::xml_node<> *node,
                      SipRec_PartiStreamAssoc & partiStreamAssocInfo)
{

    std::cout << "xGateRSXMLParser::encodePartiStreamAssoc" << std::endl;
    rapidxml::xml_node<>* parti_str_asso_node ;
    try {
       parti_str_asso_node = doc.allocate_node(rapidxml::node_element,
          RSXML_PARTI_STREAM_ASSOC_TYPE_NODE);
    } catch (const std::exception& e) {
        std::cout << "" << e.what() << std::endl;
    }
    parti_str_asso_node->append_attribute(doc.allocate_attribute(
          RSXML_PARTI_STREAM_ASSOC_TYPE_ID, partiStreamAssocInfo.participantID.c_str()));
    node->append_node(parti_str_asso_node);
    rapidxml::xml_node<>* send_node;
    try {
       send_node = doc.allocate_node(rapidxml::node_element,
              RSXML_PARTI_STREAM_ASSOC_TYPE_SEND, partiStreamAssocInfo.send.c_str());
    } catch (const std::exception& e) {
        std::cout << "" << e.what() << std::endl;
    }
    parti_str_asso_node->append_node(send_node);

    rapidxml::xml_node<>* recv_node;
    try {
        recv_node = doc.allocate_node(rapidxml::node_element,
              RSXML_PARTI_STREAM_ASSOC_TYPE_RECV, partiStreamAssocInfo.recv.c_str());
    } catch (const std::exception& e) {
        std::cout << "" << e.what() << std::endl;
    }
    parti_str_asso_node->append_node(recv_node);

}


/*
 *  This Function prints parsed RS Metadata on std output.
 */ 
void xGateRSXMLParser::printParsedXML(SipRec_RsMetadata rsMData)
{
    std::cout << "xGateRSXMLParser::printParsedXML"                                 << std::endl;
    std::cout << " xGateRSXMLParser::decodeRecordingInfo: " << rsMData.recordingNS  << std::endl;
    std::cout << " xGateRSXMLParser::DataMode            : " << rsMData.dataMode     << std::endl;
    std::cout << " xGateRSXMLParser::CallType            : " << rsMData.callType     << std::endl;
    std::cout << std::endl;
    std::cout << " xGateRSXMLParser::decodeSessions"                                 << std::endl;
    for (std::vector<SipRec_Session>::iterator i = rsMData.sessionInfo.begin();
        i != rsMData.sessionInfo.end(); ++i ) {
            std::cout << " Session::SessionId                : " << i->sessionID    << std::endl;
            std::cout << " Session::SIPSessionId             : " << i->sipSessionID << std::endl;
            std::cout << " Session::callgroupid              : " << i->groupRef     << std::endl;
    }
    std::cout << std::endl; 
    std::cout << " xGateRSXMLParser::decodeParticipants " << std::endl;
    for (std::vector<SipRec_Participant>::iterator i = rsMData.partiInfo.begin();
        i != rsMData.partiInfo.end(); ++i ) {
            std::cout << " Participant::participant_id       : " << i->participantID << std::endl;
            std::cout << " Participant::AddressOfRecord      : " << i->nameID        << std::endl;
            std::cout << " Participant::Name                 : " << i->name          << std::endl;
            std::cout << " Participant::Odr_Command          : " << i->odr           << std::endl;
            std::cout << " Participant::callSessionID_value  : " << i->callSessionID << std::endl;
    }
    std::cout <<  std::endl; 
    std::cout << " xGateRSXMLParser::decodeStreams"      << std::endl;
    for (std::vector<SipRec_Stream>::iterator i = rsMData.streamInfo.begin();
        i != rsMData.streamInfo.end(); ++i ) {
            std::cout << " Stream::stream_id                 : " << i->streamID      << std::endl;
            std::cout << " Stream::session_id                : " << i->sessionID     << std::endl;
            std::cout << " Stream::Label                     : " << i->label         << std::endl;
    }
    std::cout << std::endl;  
    std::cout << " xGateRSXMLParser::decodeSessionRecordAssoc"                       << std::endl;
    for (std::vector<SipRec_PartiSessionAssoc>::iterator i = rsMData.partiSessAssocInfo.begin();
        i != rsMData.partiSessAssocInfo.end(); ++i ) {
            std::cout << " SessionRecordAssoc::SessionID     : " << i->sessionID     << std::endl;
            std::cout << " SessionRecordAssoc::Associate_Time: " << i->associateTime << std::endl;
    }
    std::cout << std::endl;  
    std::cout << " xGateRSXMLParser::decodePartiSessionAssoc"  << std::endl;
    for (std::vector<SipRec_PartiSessionAssoc>::iterator i = rsMData.partiSessAssocInfo.begin();
        i != rsMData.partiSessAssocInfo.end(); ++i ) {
            std::cout << " PartiSessionAssoc::ParticipantID  : " << i->participantID << std::endl;
            std::cout << " PartiSessionAssoc::SessionID      : " << i->sessionID     << std::endl;
            std::cout << " PartiSessionAssoc::AssociateTime  : " << i->associateTime << std::endl;
    }
    std::cout << std::endl;  
    std::cout << " xGateRSXMLParser::decodePartiStreamAssoc" << std::endl;
    for (std::vector<SipRec_PartiStreamAssoc>::iterator i = rsMData.partiStreamAssocInfo.begin();
        i != rsMData.partiStreamAssocInfo.end(); ++i ) {
            std::cout << " PartiStreamAssoc::SessionID       : " << i->participantID << std::endl;
            std::cout << " PartiStreamAssoc::Send            : " << i->send          << std::endl;
            std::cout << " PartiStreamAssoc::Recv            : " << i->recv          << std::endl;
    }
}
