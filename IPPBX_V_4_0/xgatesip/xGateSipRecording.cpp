/*
 * ******************************************************************************
 * Vectone Mobile
 * @file xGateSipRecording.cpp
 *
 * @brief This is Sip Recording header file.
 *
 * @author Narrain prithvi Dharuman
 * ******************************************************************************
 */

#include "xGateSipRecording.h"
#include <algorithm>
#include <ctime>

/*
 * Recording constructor
 */
xGateSipRecording::xGateSipRecording() {
}

xGateSipRecording::xGateSipRecording(const char* payload)
       : m_payload(std::string(payload))
{
}

xGateSipRecording::xGateSipRecording(std::string payload)
       : m_payload(payload)
{
}

/*
 * Recording destructor
 */
xGateSipRecording::~xGateSipRecording() {
}

/*
 * This is outside exposed fucntion to request Recording mestadata for
 * SIP INFO received in dialoague.
 */
bool xGateSipRecording::infoRequest(const char* payload)
{
    if(payload != NULL) {
        m_payload = std::string(payload);
        return decodeXML();
    }
    return false;
}


/*
 * Parse the input string 'payload' and stores the required information
 * into m_confXML for further process.
*/
bool xGateSipRecording::decodeXML()
{
    SipRec_RsMetadata    m_dummyConfXML;
    if (!m_payload.empty()) {
       m_xmlParser.loadParser(m_payload.c_str());
       if ( m_xmlParser.parseXMLInfo(m_dummyConfXML) )
	  m_confXML = m_dummyConfXML;
    }

    return true;
}

/* Future Use Dont Call it
 */
SipRec_RsMetadata xGateSipRecording::getRecordingInfo()
{
    return m_confXML;
}

/* Get Parti Hold/Resume START/STOP
 *
 */

SipRecEvent xGateSipRecording::getSipInfoAction()
{
    SipRecEvent siprecEvent;

    siprecEvent.DataMode  = m_confXML.dataMode;
    siprecEvent.CallId    = "";
    siprecEvent.CallState = SipRecEvent::NONE;
    siprecEvent.CallType  = m_confXML.callType;
    siprecEvent.From      = m_confXML.from_number; 
    siprecEvent.To        = m_confXML.to_number;
    siprecEvent.Domain    = m_confXML.domain_name;
    siprecEvent.Domain    = m_confXML.domain_id;
    siprecEvent.QExtn     = m_confXML.q_extn;
    std::string Filepath,Aor_details,Direction,Extension,ccaasAgentNum(""),ccaasCoustmerNum(""),ccaasExternalNum("");
    unsigned int isCcaas = 0, ccaasDirection = 0, isCrm = 0;
  //  char *Extension ; 
    if (m_confXML.dataMode == "complete" ) // Start for all Partied or parties that are not started.
    {
        siprecEvent.CallState = SipRecEvent::NONE;
        std::vector<SipRec_PartiStreamAssoc>::iterator it = m_confXML.partiStreamAssocInfo.begin();
        for (; it != m_confXML.partiStreamAssocInfo.end(); it++) {
            //get participant call_id from RS Metadata
	    std::string call_id = base64Decode((*it).participantID);

            //Check if the participant is already started, check in the list
            std::vector<std::string>::iterator start_elem = std::find( StartedParties.begin(), StartedParties.end(), call_id);
            if(start_elem == StartedParties.end())  {
                //siprecEvent.DataMode  = m_confXML.dataMode;
                //siprecEvent.CallState = SipRecEvent::NONE;
                //siprecEvent.CallId    = base64Decode( (*it).participantID);
                StartParties.push_back(call_id);
                //break;
            }
            // NFS Metadata extraction for new user/participant


#if 1 //Logic For getting Recording Extesnion value from XML based on call_id 
	    std::vector<SipRec_Participant>::iterator itt = m_confXML.partiInfo.begin();
	    for (; itt != m_confXML.partiInfo.end(); itt++){
	     std::string parti_id = base64Decode((*itt).participantID);
	      if(strcmp(parti_id.c_str(),call_id.c_str() )==0){
               Aor_details = (*itt).nameID;
               size_t npos = Aor_details.find("@");
	       if(npos != -1)
	       {
                 Extension = Aor_details.substr(0,npos);
               }
	       if(strcmp(m_confXML.from_number.c_str(), Extension.c_str())==0)
	       {
		       m_confXML.callType=m_confXML.from_calltype;
	       }
	       else if(strcmp(m_confXML.to_number.c_str(), Extension.c_str())==0)
	       {
		       m_confXML.callType=m_confXML.to_calltype;
	       }

	       std::string str = "home/nfsshare/Recording";
	       if(strcmp(m_confXML.to_calltype.c_str(),"ccaas") == 0)
	       {
               if (!m_confXML.ccaas_recfile_name.empty())
               {
                   Filepath.append("/");
                   Filepath.append(str);
                   Filepath.append("/");
                   Filepath.append(m_confXML.ccaas_recfile_name);
               }
               isCcaas = 1;
	       }
	       else if(strcmp(m_confXML.to_calltype.c_str(),"crm") == 0)
	       {
               if (!m_confXML.ccaas_recfile_name.empty())
               {
                   Filepath.append("/");
                   Filepath.append(str);
                   Filepath.append("/");
                   Filepath.append(m_confXML.ccaas_recfile_name);
               }
               isCrm = 1;
	       }
	       else
	       {
		       Filepath = "/" + str + "/" + m_confXML.domain_name +  "/" +Extension + "/" + \
				   m_confXML.from_number + "_" + m_confXML.to_number + "_" + call_id;
	       }              
	        break;
	      }
	    }
#endif 

#if 0 //Added logic for identifying call Direction
      if(strcmp(m_confXML.from_number.c_str(), Extension)==0)
        Direction = "Outbound";
      else 
        Direction = "Inbound"; 
#endif
        if (1 == isCcaas || 1 == isCrm)
        {
            if (m_confXML.ccaas_call_type == "Outbound")
            {
                ccaasDirection = 0;
                Direction = "Outbound";
                ccaasAgentNum = m_confXML.from_number;
                ccaasCoustmerNum = m_confXML.to_number;
            }
            else if (m_confXML.ccaas_call_type == "Inbound")
            {
                ccaasAgentNum = m_confXML.to_number;
            }
            else if (m_confXML.ccaas_call_type == "External")
            {
                ccaasExternalNum = m_confXML.to_number;
            }
            if (m_confXML.ccaas_call_type != "Outbound")
            {
                ccaasDirection = 1;
                Direction = "Inbound";
                ccaasCoustmerNum = m_confXML.from_number;
            }
        }
        else
        {
            if (strncmp(m_confXML.from_number.c_str(), Extension.c_str(), m_confXML.from_number.length()) == 0)
                Direction = "Outbound";
            else if (strncmp(m_confXML.to_number.c_str(), Extension.c_str(), m_confXML.from_number.length()) == 0)
                Direction = "Inbound";
        }

            NfsMetaData nfsPartyData("",call_id, m_confXML.from_number, m_confXML.to_number, \
                 Extension, m_confXML.domain_name, m_confXML.domain_id, m_confXML.callType,\
                  Filepath, Direction, "", "" ,m_confXML.q_extn,"",0,0,isCcaas, isCrm, ccaasDirection,\
                  m_confXML.ccaas_rec_url,ccaasAgentNum,ccaasCoustmerNum,ccaasExternalNum);

            std::vector<NfsMetaData>::iterator nfsElement = std::find_if( vNfsMetaData.begin(),\
                 vNfsMetaData.end(), find_callId(nfsPartyData));
            if(nfsElement == vNfsMetaData.end()) {
                vNfsMetaData.push_back(nfsPartyData);
            }
        }

    } else if (m_confXML.dataMode == "partial" )
    {
        if (strcmp(m_confXML.to_calltype.c_str(), "ccaas") == 0)
            isCcaas = 1;
        if (1 == isCcaas && m_confXML.ccaas_call_type == "Inbound")
        {
            auto it = m_confXML.partiStreamAssocInfo.begin();
            for (; it != m_confXML.partiStreamAssocInfo.end(); it++)
            {
                // get participant call_id from RS Metadata
                std::string call_id = base64Decode((*it).participantID);
                auto nfsElement = vNfsMetaData.begin();
                for (; nfsElement != vNfsMetaData.end(); nfsElement++)
                {
                    if (strcmp((*nfsElement).CallId.c_str(), call_id.c_str()) == 0)
                    {
                        (*nfsElement).CcaasAgentNum = m_confXML.to_number;
                        break;
                    }
                }
            }
        }
    //Check datamode is partial then its hold or Resume
        siprecEvent.CallState = SipRecEvent::NONE;
        //Traverse through Parties;
        std::vector<SipRec_PartiStreamAssoc>::iterator it = m_confXML.partiStreamAssocInfo.begin();
        for (; it != m_confXML.partiStreamAssocInfo.end(); it++) {
            //get participant call_id from RS Metadata
	    std::string call_id = base64Decode((*it).participantID);

            //Check if the participant is already hold, check in the list
            std::vector<std::string>::iterator hold_elem = std::find( holdParties.begin(), holdParties.end(), call_id);

            //Received Party stream is association is receive only and its not already in hold parties list.
            //Mark party is on hold and add into hold list
            if( (*it).recv.empty() && (hold_elem == holdParties.end()) )  {
                siprecEvent.DataMode  = m_confXML.dataMode;
                siprecEvent.CallState = SipRecEvent::HOLD;
                siprecEvent.CallId    = base64Decode( (*it).participantID);
                holdParties.push_back(call_id);
                break;
            }

            //If Party receid resume and check if that party is already on hold
            if( !(*it).recv.empty() && (hold_elem != holdParties.end()) ) {
		bool found = false;
		for (int i=0; i< holdParties.size(); i++) {
		    if (holdParties[i] == call_id ) {
                       siprecEvent.DataMode  = m_confXML.dataMode;
                       siprecEvent.CallState = SipRecEvent::RESUME;
                       siprecEvent.CallId    = base64Decode( (*it).participantID);
                       holdParties.erase(holdParties.begin() + i);
                       found = true;
                       break;
		    }
		}
		if (found) break;
	    }
        } //for

        //Party SessionAssociation, mainly for PArticipant leaving from confrence
        std::vector<SipRec_PartiSessionAssoc>::iterator itr = m_confXML.partiSessAssocInfo.begin();
        for (; itr != m_confXML.partiSessAssocInfo.end(); itr++) {
	    std::string call_id = base64Decode((*itr).participantID);
            if( !(*itr).disAssociateTime.empty()) {
                siprecEvent.DataMode  = m_confXML.dataMode;
                siprecEvent.CallState = SipRecEvent::STOP;
                siprecEvent.CallId    = base64Decode( (*itr).participantID);
            }
        }
    }

    return siprecEvent;
}

/* Future Use Dont Call it
 */
std::string xGateSipRecording::getXmlPayload()
{
    return m_payload;
}


/**/

bool xGateSipRecording::encodeXML()
{
    return true;
}

bool xGateSipRecording::sipecordingStart()
{
    return true;
}
/**/

bool xGateSipRecording::sipecordingStop()
{
    return true;
}

/**/
bool xGateSipRecording::sipecordingUpdate()
{

    return true;
}

/**/

void xGateSipRecording::XMLParserTest()
{
}

/**/

void xGateSipRecording::siprecSRSRegister(bool auth=false)
{
   nua_handle_t    *reg_handle;
   char            *from_uri     = "\"SRS Recorder\"<sip:recorder@10.22.3.186>;tag=srs_fromtag";
   char            *to_uri       = "\"SRC \"<sip:recorder@10.22.3.186>;tag=srs_totag";
   char            *contact_uri  = "Contact: <sip:recorder@10.22.3.186>;+sip.srs";
   if ( !auth )
       reg_handle  =  nua_handle(this->sofia_nua, NULL, SIPTAG_TO_STR(to_uri), SIPTAG_FROM_STR(from_uri),
            SIPTAG_CONTACT_STR(contact_uri), TAG_END());
//   else
//        s2_sip_respond_to(m, NULL, SIP_401_UNAUTHORIZED, SIPTAG_WWW_AUTHENTICATE_STR(s2_auth2_digest_str),
//             SIPTAG_PROXY_AUTHENTICATE_STR(s2_auth_digest_str), TAG_END());

   nua_register(reg_handle, TAG_END());
}

const std::string xGateSipRecording::base64Decode(const void* data, const size_t &len)
{
    if (len == 0) return "";

    unsigned char *p = (unsigned char*) data;
    size_t j = 0,
           pad1 = len % 4 || p[len - 1] == '=',
           pad2 = pad1 && (len % 4 > 2 || p[len - 2] != '=');
    const size_t last = (len - pad1) / 4 << 2;
    std::string result(last / 4 * 3 + pad1 + pad2, '\0');
    unsigned char *str = (unsigned char*) &result[0];

    for (size_t i = 0; i < last; i += 4)
    {
      size_t n = B64index[p[i]] << 18 | B64index[p[i + 1]] << 12 | B64index[p[i + 2]] << 6 | B64index[p[i + 3]];
      str[j++] = n >> 16;
      str[j++] = n >> 8 & 0xFF;
      str[j++] = n & 0xFF;
    }
    if (pad1)
    {
      size_t n = B64index[p[last]] << 18 | B64index[p[last + 1]] << 12;
      str[j++] = n >> 16;
      if (pad2)
      {
        n |= B64index[p[last + 2]] << 6;
        str[j++] = n >> 8 & 0xFF;
      }
    }
    return result;
}

std::string xGateSipRecording::base64Decode(const std::string& str64)
{
    return base64Decode(str64.c_str(), str64.size());
}

