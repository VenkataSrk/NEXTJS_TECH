#include "xGatePBXCDR.h"
#include "xGateDbmMsg.h"
#include "xGateDbmDispatcher.h"
#include "log.h"
#include "klog.h"
#define THISMODULE "CDR"
xGatePBXCDR::xGatePBXCDR(){
}
xGatePBXCDR::~xGatePBXCDR(){
}
#if 0
void xGatePBXCDR::updateCDRInfo(){
}
std::string xGatePBXCDR::setJsonString(xGateCDRStruct cdrinfo){

/*curl -X POST "http://10.22.7.29:5002/api/PBX_CDR_Info" -H "accept: application/json" -H "Content-Type: application/json" -d "{ \"Uid\":\"599weact-qwert-qwert\",\"Calling_No\":\"590\",\"Called_No\":\"380\",\"Call_DateTime\":\"2020-04-15T19:20:41.052Z\",\"Connected_Time\":\"2020-04-15T19:20:41.052Z\",\"Disconnected_Time\":\"2020-04-15T19:20:41.052Z\",\"Call_Duration\":\"0:0:0\",\"Domain_Name\":\"6367.UR.mundio.com\",\"Domain_ID\":1674,\"CallFeature\":\"INTERNAL\",\"PBXIP\":\"10.22.7.141\",\"PBXID\":1,\"MGIP\":\"10.22.7.118\",\"MGID\":1,\"bundleID\":0,\"operatorIP\":\"\",\"calling_codec\":\"G711\",\"called_coded\":\"G711\",\"calltype\":\"INBOUND\",\"holdDuration\":\"0:00\",\"callforwarded\":false,\"callforward_No\":\"\",\"Disconnect_Reason\":\"NORMAL\",\"DTMF_Pressed\":\"\",\"call_recording\":false,\"recordingtype\":0,\"CallTransfer\":false,\"Transfer_No\":\"\",\"Conference_Type\":\"\",\"Conf_joiner_type\":0,\"Conference_Number\":\"\",\"Conference_Ext\":\"\" }"*/
std::ostringstream oss;
oss << jbegin << "Uid" << jcolon << cdrinfo.uid << jcomma << "Calling_No" << jcolon << cdrinfo.callingNo << jcomma \
 << "Called_No" << jcolon << cdrinfo.calledNo << jcomma << "Call_DateTime" << jcolon << cdrinfo.callDateTime << jcomma \
 << "Connected_Time" << jcolon << cdrinfo.connectedTime << jcomma \
 << "Disconnected_Time" << jcolon << cdrinfo.disconnectedTime << jcomma \
 << "Call_Duration"<< jcolon << cdrinfo.callDuration << jcomma << "Domain_Name" << jcolon << cdrinfo.domainName << jcomma \
 << "Domain_ID" << jcolon << cdrinfo.domainID << jcomma  << "CallFeature"<< jcolon << cdrinfo.callFeature << jcomma \
 << "PBXIP" << jcolon << cdrinfo.pbxip << jcomma << "PBXID" << jcolon << cdrinfo.pbxid << jcomma \
 << "MGIP" << jcolon << cdrinfo.mgip << jcomma << "MGID" << jcolon << cdrinfo.mgid << jcomma \
 << "bundleID" << jcolon << cdrinfo.bundleID << jcomma << "operatorIP" << jcolon << cdrinfo.operatorIP << jcomma \
 << "calling_codec" << jcolon << cdrinfo.callingCodec << jcomma << "called_coded" << jcolon << cdrinfo.calledCoded << jcomma \
 << "calltype"<< jcolon << cdrinfo.callType << jcomma << "holdDuration" << jcolon << cdrinfo.holdDuration << jcomma \
 << "callforwarded"<< jcolon << cdrinfo.callForwarded << jcomma << "callforward_No" << jcolon << cdrinfo.callForwardNo << jcomma \
 << "Disconnect_Reason"<< jcolon << cdrinfo.disconnectReason << jcomma << "DTMF_Pressed" << jcolon << cdrinfo.digitPressed << jcomma \
 << "call_recording"<< jcolon << cdrinfo.callRecording << jcomma << "recordingtype" << jcolon << cdrinfo.recordingType << jcomma \
 << "CallTransfer"<< jcolon << cdrinfo.callTransfer << jcomma << "Transfer_No" << jcolon << cdrinfo.transferNo << jcomma \
 << "Conference_Type" << jcolon << cdrinfo.conferenceType << jcomma \
 << "Conf_joiner_type"<< jcolon << cdrinfo.confJoinerType << jcomma \
 << "Conference_Number"<< jcolon << cdrinfo.conferenceNumber << jcomma \
 << "Conference_Ext"<< jcolon << cdrinfo.conferenceExt << jcomma << "GrpID" << cdrinfo.grpid << jend;
return oss.str();
}
#endif
void xGatePBXCDR::postMessage(std::string input,std::string uid){

xGateDbmMsg *pMongoDBMDispatcher = new xGateDbmMsg();
  pMongoDBMDispatcher->setMsgType(EN_XGATE_MSG_TO_DBM);
  DbmReqDetail &reqDetail = pMongoDBMDispatcher->get_dbm_request_detail();
  reqDetail.m_transId = "123456789";//uid;
  reqDetail.m_dbType = EN_XGATE_DB_LAYER_MONGODB;
  reqDetail.m_opCode = EN_XGATE_DBM_MSG_DBEXECUTE;
  reqDetail.m_dbName = "unifiedring_pbx";
  reqDetail.m_spName = "PBX_CDR_Info";
  reqDetail.m_url = "http://10.22.7.29:5002/api/PBX_CDR_Info";
  reqDetail.m_tableName = "PBX_CDR_TEMP";

  //reqDetail.m_input = setJsonString(cdrinfo);
  reqDetail.m_input = input;

  ACE_Message_Block* pAmb = NULL;
  pAmb = static_cast<xGateDbmMsg*>(pMongoDBMDispatcher);
  ACE_Time_Value tval ((time(NULL) ));
  tval += ACE_Time_Value(0,1);

  ACE_Task<ACE_MT_SYNCH>* taskPtr = xGateUtil::getDbmDispatcher();
  if(taskPtr->putq(pAmb, &tval)) {
    CLog::Detail(THISMODULE, "MGC-Business-layer-thread posted message to DBM-Network-component %s ",uid.c_str());
  } else {
    CLog::Error(THISMODULE, "MGC-Business-layer-thread failed to pose message to DBM-Network-component %s!",uid.c_str());
  }

}
