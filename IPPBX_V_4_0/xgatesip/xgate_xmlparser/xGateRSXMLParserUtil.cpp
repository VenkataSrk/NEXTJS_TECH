
/*
 * ******************************************************************************
 * Vectone Mobile
 * @file xGateRSXMLParserUtil.cpp
 *
 * @brief This is RS Metadata Info XML for storing Parser/Encoder DataStructure. 
 *
 * @author Narrain prithvi Dharuman
 * ******************************************************************************
 */

#include "xGateRSXMLParserUtil.h"


SipRec_Session::SipRec_Session(){}
SipRec_Session::~SipRec_Session(){}
SipRec_Session::SipRec_Session(const SipRec_Session & rhs)
    :    sessionID(rhs.sessionID),
         sipSessionID(rhs.sipSessionID),
         groupRef(rhs.groupRef)

{
}
SipRec_Session & SipRec_Session::operator=(const SipRec_Session & rhs)
{
    sessionID       =  rhs.sessionID;
    sipSessionID    =  rhs.sipSessionID;
    groupRef        =  rhs.groupRef;

    return *this;

}

SipRec_Participant::SipRec_Participant(){}
SipRec_Participant::~SipRec_Participant(){}
SipRec_Participant::SipRec_Participant(const SipRec_Participant & rhs)
     :   participantID(rhs.participantID),
         name(rhs.name),
         nameID(rhs.nameID),
         callingParty(rhs.callingParty),
         odr(rhs.odr),
         callSessionID(rhs.callSessionID)
{
}

SipRec_Participant & SipRec_Participant::operator=(const SipRec_Participant & rhs)
{
     participantID = rhs.participantID;
     name          = rhs.name;
     nameID        = rhs.nameID;
     callingParty  = rhs.callingParty;
     odr           = rhs.odr;
     callSessionID = rhs.callSessionID;

     return *this;
}


SipRec_Stream::SipRec_Stream(){}
SipRec_Stream::~SipRec_Stream(){}
SipRec_Stream::SipRec_Stream(const SipRec_Stream & rhs)
     :  streamID(rhs.streamID),
        sessionID(rhs.sessionID),
        label(rhs.label)
{
}
SipRec_Stream & SipRec_Stream::operator=(const SipRec_Stream & rhs)
{
    streamID    = rhs.streamID;
    sessionID   = rhs.sessionID;
    label       = rhs.label;

    return *this;
}

SipRec_SessionRecAssoc::SipRec_SessionRecAssoc(){}
SipRec_SessionRecAssoc::~SipRec_SessionRecAssoc(){}

SipRec_SessionRecAssoc::SipRec_SessionRecAssoc(const SipRec_SessionRecAssoc & rhs)
     : sessionID(rhs.sessionID),
       associateTime(rhs.associateTime)
 {
 }

SipRec_SessionRecAssoc & SipRec_SessionRecAssoc::operator=(const SipRec_SessionRecAssoc & rhs)
 {
     sessionID     = rhs.sessionID;
     associateTime = rhs.associateTime;

    return *this;
}


SipRec_PartiSessionAssoc::SipRec_PartiSessionAssoc(){}
SipRec_PartiSessionAssoc::~SipRec_PartiSessionAssoc(){}
SipRec_PartiSessionAssoc::SipRec_PartiSessionAssoc(const SipRec_PartiSessionAssoc & rhs)
    :  participantID(rhs.participantID),
       sessionID(rhs.sessionID),
       associateTime(rhs.associateTime),
       disAssociateTime(rhs.disAssociateTime)
{
}

SipRec_PartiSessionAssoc & SipRec_PartiSessionAssoc::operator=(const SipRec_PartiSessionAssoc & rhs)
{
    participantID = rhs.participantID;
    sessionID     = rhs.sessionID;
    associateTime = rhs.associateTime;
    disAssociateTime = rhs.disAssociateTime;
    return *this;
}



SipRec_PartiStreamAssoc::SipRec_PartiStreamAssoc(){}
SipRec_PartiStreamAssoc::~SipRec_PartiStreamAssoc(){}

SipRec_PartiStreamAssoc::SipRec_PartiStreamAssoc(const SipRec_PartiStreamAssoc & rhs)
    :  participantID(rhs.participantID),
       send(rhs.send),
       recv(rhs.recv)
{
}

SipRec_PartiStreamAssoc & SipRec_PartiStreamAssoc::operator=(const SipRec_PartiStreamAssoc & rhs)
{
    participantID = rhs.participantID;
    send          = rhs.send;
    recv          = rhs.recv;
    return *this;
}



SipRec_RsMetadata::SipRec_RsMetadata() {}
SipRec_RsMetadata::~SipRec_RsMetadata(){
}
// Copy constructor
SipRec_RsMetadata::SipRec_RsMetadata(const SipRec_RsMetadata &rhs) :
        recordingNS(rhs.recordingNS),
        dataMode(rhs.dataMode),
        from_number(rhs.from_number),
        to_number(rhs.to_number),
	from_calltype(rhs.from_calltype),
	to_calltype(rhs.to_calltype),
        domain_name(rhs.domain_name),
	domain_id(rhs.domain_id),
	q_extn(rhs.q_extn),
	ccaas_recfile_name(rhs.ccaas_recfile_name),
        ccaas_rec_url(rhs.ccaas_rec_url),
        ccaas_call_type(rhs.ccaas_call_type),
        sessionInfo(rhs.sessionInfo),
        partiInfo(rhs.partiInfo),
        streamInfo(rhs.streamInfo),
        sessRecAssocInfo(rhs.sessRecAssocInfo),
        partiSessAssocInfo(rhs.partiSessAssocInfo),
        partiStreamAssocInfo(rhs.partiStreamAssocInfo)

{
}

// // Copy assignment operator
SipRec_RsMetadata & SipRec_RsMetadata::operator=(const SipRec_RsMetadata & rhs)
{
    recordingNS            = rhs.recordingNS;
    dataMode               = rhs.dataMode;
    from_number            = rhs.from_number;
    to_number              = rhs.to_number;
    from_calltype          = rhs.from_calltype;
    to_calltype            = rhs.to_calltype;
    domain_name            = rhs.domain_name;
    domain_id              = rhs.domain_id;
    q_extn                 = rhs.q_extn;
    ccaas_recfile_name     = rhs.ccaas_recfile_name;
    ccaas_rec_url          = rhs.ccaas_rec_url;
    ccaas_call_type        = rhs.ccaas_call_type;
    sessionInfo            = rhs.sessionInfo;
    partiInfo              = rhs.partiInfo;
    streamInfo             = rhs.streamInfo;
    sessRecAssocInfo       = rhs.sessRecAssocInfo;
    partiSessAssocInfo     = rhs.partiSessAssocInfo;
    partiStreamAssocInfo   = rhs.partiStreamAssocInfo;

    return *this;
}

NfsMetaData::NfsMetaData()
{
};
NfsMetaData::NfsMetaData(std::string CallUid,std::string callId, std::string from, std::string to, \
    std::string  extension, std::string domain, std::string domainId, std::string callType,\
    std::string filePath ,std::string direction, unsigned long  mp3_duration, unsigned long mp3_filesize,\
    std::string qExtn,std::string createdtime,unsigned int recStartTime,unsigned int recStopTime,unsigned int isCcaas, unsigned int isCrm,\
    unsigned int ccaasDirection,std::string ccaasRecUrl, std::string ccaasAgentNum, std::string ccaasCoustmerNum, std::string ccaasExternalNum) :
    CallUid           (CallUid),
    CallId            (callId),
    From              (from),
    To                (to),
    Extension         (extension),
    Domain            (domain),
    DomainId          (domainId),
    CallType          (callType),
    FilePath          (filePath),
    Direction         (direction),
    FileDuration      (mp3_duration),
    FileSize          (mp3_filesize),
    QExtn             (qExtn),
    CreatedTime       (createdtime),
    CcaasAgentNum     (ccaasAgentNum),
    CcaasCoustmerNum  (ccaasCoustmerNum),
    CcaasExternalNum  (ccaasExternalNum),
    RecStartTime      (recStartTime),
    RecStopTime       (recStopTime),
    IsCcaas           (isCcaas),
    IsCrm             (isCrm),
    CcaasDirection    (ccaasDirection),
    CcaasRecUrl       (ccaasRecUrl)
{
}
NfsMetaData::~NfsMetaData()
{
}

// Copy constructor
NfsMetaData::NfsMetaData(const NfsMetaData& rhs) :
    
    CallUid           (rhs.CallUid),
    CallId            (rhs.CallId),
    From              (rhs.From),
    To                (rhs.To),
    Extension         (rhs.Extension),
    Domain            (rhs.Domain),
    DomainId          (rhs.DomainId),
    CallType          (rhs.CallType),
    FilePath          (rhs.FilePath),
    Direction         (rhs.Direction),
    FileDuration      (rhs.FileDuration),
    FileSize          (rhs.FileSize),
    QExtn	          (rhs.QExtn),
    CreatedTime       (rhs.CreatedTime),
    CcaasAgentNum     (rhs.CcaasAgentNum),
    CcaasCoustmerNum  (rhs.CcaasCoustmerNum),
    CcaasExternalNum  (rhs.CcaasExternalNum),
    RecStartTime      (rhs.RecStartTime),
    RecStopTime       (rhs.RecStopTime),
    IsCcaas           (rhs.IsCcaas),
    IsCrm             (rhs.IsCrm),
    CcaasDirection    (rhs.CcaasDirection),
    CcaasRecUrl       (rhs.CcaasRecUrl)
{
}
// // Copy assignment operator
NfsMetaData & NfsMetaData::operator=(const NfsMetaData& rhs)
{
    CallUid           =    rhs.CallUid;
    CallId            =    rhs.CallId;
    From              =    rhs.From;
    To                =    rhs.To;
    Extension         =    rhs.Extension;
    Domain            =    rhs.Domain;
    DomainId          =    rhs.DomainId;
    CallType          =    rhs.CallType;
    FilePath          =    rhs.FilePath;
    Direction         =    rhs.Direction;
    FileDuration      =    rhs.FileDuration;
    FileSize          =    rhs.FileSize;
    QExtn             =    rhs.QExtn;
    CreatedTime       =    rhs.CreatedTime;
    CcaasAgentNum     =    rhs.CcaasAgentNum;
    CcaasCoustmerNum  =    rhs.CcaasCoustmerNum;
    CcaasExternalNum  =    rhs.CcaasExternalNum;
    RecStartTime      =    rhs.RecStartTime;
    RecStopTime       =    rhs.RecStopTime;
    IsCcaas           =    rhs.IsCcaas;
    IsCrm             =    rhs.IsCrm;
    CcaasDirection    =    rhs.CcaasDirection;
    CcaasRecUrl       =    rhs.CcaasRecUrl;

    return *this;
}

//Output Stream Operator Overloading for printing
std::ostream& operator<<(std::ostream& os, const NfsMetaData& dt)
{
    ///os << dt.mo << '/' << dt.da << '/' << dt.yr;
     os << "Calluid   : " <<  dt.CallUid   <<"\n"; 
     os << "CallId    : " <<  dt.CallId   <<"\n"; 
     os << "From      : " <<  dt.From     <<"\n";  
     os << "To        : " <<  dt.To       <<"\n";   
     os << "Extension : " <<  dt.Extension<<"\n"; 
     os << "Domain    : " <<  dt.Domain   <<"\n";  
     os << "DomainId  : " <<  dt.DomainId <<"\n"; 
     os << "CallType  : " <<  dt.CallType <<"\n";  
     os << "FilePath  : " <<  dt.FilePath <<"\n";   
     os << "Direction : " <<  dt.Direction<<"\n";   
     os << "FileDuration : " <<  dt.FileDuration<<"\n";   
     os << "FileSize : " <<  dt.FileSize<<"\n";   
     os << "QExtn     : " <<  dt.QExtn    <<"\n";
     os << "CreatedTime : " << dt.CreatedTime <<"\n";
    return os;
}

//bool NfsMetaData::operator () ( const NfsMetaData& nfs ) const
//{
//    return nfs.CallId == CallId;
//}
bool NfsMetaData::operator==(const NfsMetaData& rhs)
{
    return CallId == rhs.CallId;
}
