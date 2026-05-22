/*
 * ******************************************************************************
 * Vectone Mobile
 * @file xGateRSXMLParserUtil.h
 *
 * @brief This is RS Metadata Info XML for storing Parser/Encoder DataStructure. 
 *
 * @author Narrain prithvi Dharuman
 * ******************************************************************************
 */
#ifndef XGATE_RS_XML_PARSER_UTILITY
#define XGATE_RS_XML_PARSER_UTILITY
#include <string>
#include <iostream>
#include <vector>

struct SipRec_CommSessionGroup {
    std::string      associatedTime;
    std::string      disassociatedTime; // *Note: it has to be callculated.
    std::string      callCenter; //Not sure we need this. but we need Supervisor
    std::string      myData;
};

struct SipRec_Session {
    std::string      sessionID;
    std::string      sipSessionID;
    std::string      groupRef;

    SipRec_Session();
    ~SipRec_Session();
    SipRec_Session(const SipRec_Session & rhs);
    SipRec_Session & operator=(const SipRec_Session & rhs);
};

struct SipRec_Participant {
    std::string      participantID;
    std::string      name;
    std::string      nameID;
    std::string      callingParty;
    std::string      odr;
    std::string      callSessionID;

    SipRec_Participant();
    ~SipRec_Participant();
    SipRec_Participant(const SipRec_Participant & rhs);
    SipRec_Participant & operator=(const SipRec_Participant & rhs);
};

struct SipRec_Stream {
    std::string      streamID;
    std::string      sessionID;
    std::string      label;

    SipRec_Stream();
    ~SipRec_Stream();
    SipRec_Stream(const SipRec_Stream & rhs);
    SipRec_Stream & operator=(const SipRec_Stream & rhs);
};

struct SipRec_SessionRecAssoc {
    std::string      sessionID;
    std::string      associateTime;

    SipRec_SessionRecAssoc();
    ~SipRec_SessionRecAssoc();
    SipRec_SessionRecAssoc(const SipRec_SessionRecAssoc & rhs);
    SipRec_SessionRecAssoc & operator=(const SipRec_SessionRecAssoc & rhs);
};


struct SipRec_PartiSessionAssoc {
    std::string      participantID;
    std::string      sessionID;
    std::string      associateTime;
    std::string      disAssociateTime;

    SipRec_PartiSessionAssoc();
    ~SipRec_PartiSessionAssoc();
    SipRec_PartiSessionAssoc(const SipRec_PartiSessionAssoc & rhs);
    SipRec_PartiSessionAssoc & operator=(const SipRec_PartiSessionAssoc & rhs);
};

struct SipRec_PartiStreamAssoc {
    std::string      participantID;
    std::string      send;
    std::string      recv;

    SipRec_PartiStreamAssoc();
    ~SipRec_PartiStreamAssoc();
    SipRec_PartiStreamAssoc(const SipRec_PartiStreamAssoc & rhs);
    SipRec_PartiStreamAssoc & operator=(const SipRec_PartiStreamAssoc & rhs);
};


struct SipRec_RsMetadata{
    std::string                recordingNS;
    std::string                dataMode;
    std::string                callType;
    std::string                from_number;
    std::string                to_number;
    std::string 	       from_calltype;
    std::string 	       to_calltype;
    std::string                domain_name;
    std::string                domain_id;
    std::string 	       q_extn;
    std::string 	       ccaas_recfile_name;
    std::string             ccaas_rec_url;
    std::string             ccaas_call_type;
    std::vector<SipRec_Session>            sessionInfo;
    std::vector<SipRec_Participant>        partiInfo;
    std::vector<SipRec_Stream>             streamInfo;
    std::vector<SipRec_SessionRecAssoc>    sessRecAssocInfo;
    std::vector<SipRec_PartiSessionAssoc>  partiSessAssocInfo;
    std::vector<SipRec_PartiStreamAssoc>   partiStreamAssocInfo;

    SipRec_RsMetadata();
    ~SipRec_RsMetadata();
    // Copy constructor
    SipRec_RsMetadata(const SipRec_RsMetadata& rhs);
    SipRec_RsMetadata & operator=(const SipRec_RsMetadata& rhs);

};

struct NfsMetaData {
    std::string             CallUid;
    std::string             CallId;
    std::string             From;
    std::string             To;
    std::string             Extension;   
    std::string             Domain;
    std::string             DomainId;
    std::string             CallType;
    std::string             FilePath;
    std::string             Direction;
    std::string             QExtn;
    std::string             CcaasRecUrl;
    std::string             CreatedTime;
    std::string             CcaasAgentNum;
    std::string             CcaasCoustmerNum;
    std::string             CcaasExternalNum;
    unsigned int            RecStartTime;
    unsigned int            RecStopTime;
    unsigned int            IsCcaas;
    unsigned int            IsCrm;
    unsigned int            CcaasDirection;
    unsigned long           FileDuration;
    unsigned long           FileSize;
    NfsMetaData ();
    NfsMetaData(std::string CallUid,std::string callId, std::string from, std::string to, \
       std::string extesnion, std::string domain, std::string domainId, std::string callType, \
       std::string filePath , std::string direction, unsigned long fileduration, unsigned long filesize,\
       std::string qExtn,std::string createdtime,unsigned int recStartTime,unsigned int recStopTime,unsigned int isCcaas, unsigned int isCrm,\
       unsigned int ccaasDirection,std::string ccaasRecUrl,std::string ccaasAgentNum, std::string ccaasCoustmerNum, std::string ccaasExternalNum);
    ~NfsMetaData();   
    //Copy constructor
    NfsMetaData (const NfsMetaData &rhs);
    NfsMetaData & operator = (const NfsMetaData& rhs);
    bool operator () ( const NfsMetaData& nfs ) const;
    //friend std::ostream & operator << (std::ostream& os, const Date& dt);
    bool operator==(const NfsMetaData& rhs);
};

#endif
