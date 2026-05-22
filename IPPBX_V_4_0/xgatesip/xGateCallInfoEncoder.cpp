//system includes
#include <string>
#include <iostream>
#include <sstream>

//local includes
//#include "log.h"
//#include "klog.h"
#include "xGateCallInfoEncoder.h"

#define THISMODULE "CallInfoEncoder"

xGateCallInfoEncoder::xGateCallInfoEncoder()
{
}

xGateCallInfoEncoder::~xGateCallInfoEncoder()
{
}

Value xGateCallInfoEncoder::setJsonParam(string &strValue, Document &doc)
{
  Value retVal;
  retVal.SetString(strValue.c_str(), strValue.length(), doc.GetAllocator());
  return retVal;
}

Value xGateCallInfoEncoder::setJsonParam(unsigned int iValue, Document &doc)
{
  Value retVal;
  retVal = iValue;
  return retVal;
}

/***************************************************************
 * *Name : encode_call_info_json_msg
 * *Description: form json data from structure 
 * *Input: Recording NFS data 
 * *Return: string json data
 * *parameter info: call info details
 * *************************************************************/

std::string xGateCallInfoEncoder::encode_call_info_json_msg(NfsMetaData ptnfsmetadata)
{
  Document doc;
  StringBuffer strBuf;
  Writer<StringBuffer> writer(strBuf);
  Value outputVal(kObjectType);
  Document::AllocatorType &allocator = doc.GetAllocator();
  string callDuration("");
  // string callRecordingDuration("");

#if 1
  outputVal.AddMember("call_uid", setJsonParam(ptnfsmetadata.CallUid, doc), allocator);
  outputVal.AddMember("call_callid", setJsonParam(ptnfsmetadata.CallId, doc), allocator);
  outputVal.AddMember("extension", setJsonParam(ptnfsmetadata.Extension, doc), allocator);
  outputVal.AddMember("from_number", setJsonParam(ptnfsmetadata.From, doc), allocator);
  outputVal.AddMember("to_number", setJsonParam(ptnfsmetadata.To, doc), allocator);
  outputVal.AddMember("domain_name", setJsonParam(ptnfsmetadata.Domain, doc), allocator);
  outputVal.AddMember("domain_id", setJsonParam(ptnfsmetadata.DomainId, doc), allocator);
  outputVal.AddMember("call_direction", setJsonParam(ptnfsmetadata.Direction, doc), allocator);
  outputVal.AddMember("call_type", setJsonParam(ptnfsmetadata.CallType, doc), allocator);
  outputVal.AddMember("recording_duration", setJsonParam(ptnfsmetadata.FileDuration, doc), allocator);
  outputVal.AddMember("call_duration", setJsonParam(callDuration, doc), allocator);
  // outputVal.AddMember("mp3_duration", setJsonParam(ptnfsmetadata.Mp3Duration, doc), allocator);
  outputVal.AddMember("file_size", setJsonParam(ptnfsmetadata.FileSize, doc), allocator);
  outputVal.AddMember("q_extn", setJsonParam(ptnfsmetadata.QExtn, doc), allocator);
  outputVal.AddMember("created_date",setJsonParam(ptnfsmetadata.CreatedTime,doc),allocator);
  //outputVal.AddMember("rec_start_time",setJsonParam(ptnfsmetadata.RecStartTime,doc),allocator);
  //outputVal.AddMember("rec_stop_time",setJsonParam(ptnfsmetadata.RecStopTime,doc),allocator);

  Value nfsPathVal(kObjectType);
  nfsPathVal.AddMember("nfs_path", setJsonParam(ptnfsmetadata.FilePath, doc), allocator);
  outputVal.AddMember("recording_file_path", nfsPathVal, allocator);

  outputVal.Accept(writer);

  string nfsJsonData("");
  nfsJsonData = strBuf.GetString();
#endif
  return nfsJsonData;
}

std::string xGateCallInfoEncoder::encode_call_info_db_msg(NfsMetaData ptnfsmetadata)
{
	std::stringstream ss{};

	ss << squoted << ptnfsmetadata.CallUid   	<< sqcomma 
				  << ptnfsmetadata.CallId    	      << sqcomma
				  << ptnfsmetadata.Extension 	      << sqcomma
				  << ptnfsmetadata.From      	      << sqcomma
				  << ptnfsmetadata.To        	      << sqcomma
				  << ptnfsmetadata.Domain    	      << squoted << comma
				  << ptnfsmetadata.DomainId  	      << comma   << squoted
				  << ptnfsmetadata.QExtn 		        << sqcomma
				  << ptnfsmetadata.Direction 	      << sqcomma
				  << ptnfsmetadata.CallType  	      << squoted << comma
				  << ptnfsmetadata.FileDuration     << comma
				  << ptnfsmetadata.FileSize 	      << comma   << squoted
				  << ptnfsmetadata.FilePath 	      << squoted;

	return ss.str();

}

std::string xGateCallInfoEncoder::encode_ccaas_call_info_db_msg(NfsMetaData ptnfsmetadata)
{
  XGLOG_INFO("%s() Entry",__func__);
  std::string strAgentNum = ptnfsmetadata.CcaasAgentNum;
  std::string strExternalNum = ptnfsmetadata.CcaasExternalNum;
  if (strAgentNum.empty())
    strAgentNum = "0";
  if (strExternalNum.empty())
    strExternalNum = "0";

  std::stringstream ss{};
  ss << ptnfsmetadata.DomainId             << comma << squoted
     << ptnfsmetadata.CallUid              << sqcomma
     << ptnfsmetadata.CcaasRecUrl          << sqcomma
     << ptnfsmetadata.FileSize             << squoted << comma
     << ptnfsmetadata.RecStartTime         << comma
     << ptnfsmetadata.RecStopTime          << comma << squoted
     << ptnfsmetadata.FileDuration         << squoted << comma
     << ptnfsmetadata.CcaasDirection       << comma
     << strAgentNum                        << comma << squoted
     << ptnfsmetadata.CcaasCoustmerNum     << sqcomma
     << strExternalNum                     << squoted;

    return ss.str();
}

std::string xGateCallInfoEncoder::encode_crm_call_info_db_msg(NfsMetaData ptnfsmetadata)
{
  XGLOG_INFO("%s() Entry",__func__);
  std::string strAgentNum = ptnfsmetadata.CcaasAgentNum;
  std::string strExternalNum = ptnfsmetadata.CcaasExternalNum;
  if (strAgentNum.empty())
    strAgentNum = "0";
  if (strExternalNum.empty())
    strExternalNum = "0";

  std::stringstream ss{};
  ss << ptnfsmetadata.DomainId             << comma << squoted
     << ptnfsmetadata.CallUid              << sqcomma
     << ptnfsmetadata.CcaasRecUrl          << sqcomma
     << ptnfsmetadata.FileSize             << squoted << comma
     << ptnfsmetadata.RecStartTime         << comma
     << ptnfsmetadata.RecStopTime          << comma << squoted
     << ptnfsmetadata.FileDuration         << squoted << comma
     << ptnfsmetadata.CcaasDirection       <<"::smallint"<< comma
     << strAgentNum                        << comma << squoted
     << ptnfsmetadata.CcaasCoustmerNum     << sqcomma
     << strExternalNum                     << squoted;

    return ss.str();
}