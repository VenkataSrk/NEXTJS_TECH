#include "SofiaSiphandler.h"

using std::bad_alloc;

#define XGATE_USER_AGENT "UnifiedRingEngine Signaling Gateway"
#define SETPARAM SIPTAG_ALLOW_STR("INVITE, ACK, BYE, CANCEL, OPTIONS, MESSAGE, INFO"), \
				 SIPTAG_USER_AGENT_STR(XGATE_USER_AGENT),                              \
				 NUTAG_MEDIA_ENABLE(0),                                                \
				 NUTAG_AUTOANSWER(0),                                                  \
				 NUTAG_AUTOACK(1),                                                     \
				 NUTAG_AUTOALERT(0),                                                   \
				 NTATAG_EXTRA_100(0),                                                  \
				 NTATAG_REL100(0),                                                     \
				 NUTAG_ALLOW("REGISTER"),                                              \
				 NUTAG_ALLOW("INVITE"),                                                \
				 NUTAG_ALLOW("ACK"),                                                   \
				 NUTAG_ALLOW("CANCEL"),                                                \
				 NUTAG_ALLOW("OPTIONS"),                                               \
				 NUTAG_ALLOW("BYE"),                                                   \
				 NUTAG_ALLOW("REFER"),                                                 \
				 NUTAG_ALLOW("SUBSCRIBE"),                                             \
				 NUTAG_ALLOW("NOTIFY"),                                                \
				 NUTAG_ALLOW_EVENTS("talk"),                                           \
				 NUTAG_ALLOW_EVENTS("hold"),                                           \
				 NUTAG_ALLOW_EVENTS("conference"),                                     \
				 NUTAG_ALLOW_EVENTS("presence"),                                       \
				 NUTAG_ALLOW_EVENTS("presence.winfo"),                                 \
				 NUTAG_APPL_METHOD("REGISTER"),                                        \
				 NUTAG_APPL_METHOD("OPTIONS"),                                         \
				 TAG_END()
#define URLSCHEMA "sip"
#define INBOUND_CALL_MIN_LEN 6

bool StringExists(const char *p)
{
	if (p != nullptr)
		if (strlen(p))
			return true;
	return false;
}

SofiaSiphandler::SofiaSiphandler() : xGateSipController()
{
}
SofiaSiphandler::~SofiaSiphandler()
{
}
// public Implemetation SofiaSipHandler
/* Method Name: SipLibInit
 * Method used to initilize sip handler & sip stack
 */

xGateReturn SofiaSiphandler::SipLibInit()
{
	XGLOG_INFO("SofiaSiphandler::SipLibInit()");
	try
	{
		// Log Initiating sofia Libarary
    	LoadSipHandlerParams();////Loading Sip params from Config 
		SofiaInit();				   // Initltilizing sofia Libary
		SofiaInitMemory(m_homeMemory); // Creating Memory for sofia Libary
		if ((m_root = SofiaInitRoot(this)) == nullptr)
		{
			// Log Error Capture
			return xGateReturn::FAIL;
		} // Creating Root Instance
	}
	catch (...)
	{
		XGLOG_WARN("SofiaSiphandler::SipLibInit(%s)", "Exception Occured");
		return xGateReturn::FAIL;
	}
	XGLOG_INFO("SofiaSiphandler::SipLibInit(%s)", "Success-Init");
	return xGateReturn::SUCCESS;
}

xGateReturn SofiaSiphandler::SipLibLoad()
{
	XGLOG_INFO("SofiaSiphandler::SipLibLoad()");
	try
	{

		SofiaSetLog(9); // set log level for sofia library
		if ((m_nuaHandle = SofiaCreate(m_root, &SofiaSiphandler::SofiaCallBack, this)) == nullptr)
		{
			XGLOG_WARN("SofiaSiphandler::SipLibLoad(%s)", "Sofia Handle Null Return");

			return xGateReturn::FAIL;
		}
	}
	catch (...)
	{
		XGLOG_WARN("SofiaSiphandler::SipLibLoad(%s)", "Exception Occured");
		return xGateReturn::FAIL;
	}
	XGLOG_INFO("SofiaSiphandler::SipLibLoad(%s)", "Success-Loaded");
	// SendRegister();
	return xGateReturn::SUCCESS;
}

xGateReturn SofiaSiphandler::SipLibUnLoad()
{
	try
	{
		if (m_nuaHandle)
		{
			SofiaDestory(m_nuaHandle);
			SofiaDeInitRoot(m_root);
			SofiaDeInitMemory(m_homeMemory);
			SofiaDeInit();
		}
	}
	catch (...)
	{
		XGLOG_ERROR("SofiaSiphandler::SipUnLibLoad is Failed");
		return xGateReturn::FAIL;
	}
	return xGateReturn::SUCCESS;
}

xGateReturn SofiaSiphandler::RunEventLoop()
{
	try
	{
		GetProcessRegisterEvent();
		GetProcessEvent();
		SofiaRootStep(m_nuaHandle, m_root);
		//SofiaRootRun(m_root);
	}
	catch (...)
	{
		XGLOG_INFO("SofiaSiphandler::RunEventLoop(%s)", "Exception Occured");
		return xGateReturn::FAIL;
	}
	return xGateReturn::SUCCESS;
}

xGateReturn SofiaSiphandler::RespondWith1xx(xGateSofiaobj *pSipMgc)
{
	XGLOG_FUNCTION_EN_EX("");

	if (!pSipMgc)
	{
		XGLOG_ERROR("SofiaSiphandler::RespondWith1xx Invalid pSipMgc");
		return xGateReturn::FAIL;
	}

	sdp_session_t *rsdp = (sdp_session_t *)pSipMgc->rsdp;

	switch (pSipMgc->sipeventtype)
	{
	case XGATESIPEVENT::EN_100_REL:
		SofiaRespond((nua_handle_t *)pSipMgc->sofianh, EN_100_REL, "Trying");
		break;
	case XGATESIPEVENT::EN_180_REL:
	{
		if (pSipMgc->m_sAnsSdp != "")
		{
			XGLOG_INFO("SofiaSiphandler::RespondWith1xx pSipMgc->rsdp is not empty for (%s)", pSipMgc->uid.c_str());

			nua_set_hparams((nua_handle_t *)pSipMgc->sofianh,
							TAG_END());

			nua_set_hparams((nua_handle_t *)pSipMgc->sofianh, NUTAG_EARLY_MEDIA(1),
							NUTAG_EARLY_ANSWER(1),
							NUTAG_AUTOANSWER(0),
							TAG_NULL());

			// nua_set_hparams((nua_handle_t*)pSipMgc->sofianh,SIPTAG_CONTACT_STR(sContact.c_str()),TAG_NULL());

			// SofiaRespond((nua_handle_t*)pSipMgc->sofianh,EN_180_REL,"Ringing");
			/*
				 nua_respond((nua_handle_t*)pSipMgc->sofianh,180,"Ringing",
				 TAG_IF(!pSipMgc->m_sAnsSdp.empty(),SIPTAG_CONTENT_TYPE_STR("application/sdp")),
				 TAG_IF(!pSipMgc->m_sAnsSdp.empty(),SIPTAG_PAYLOAD_STR(pSipMgc->m_sAnsSdp.c_str())),
				 SIPTAG_CONTACT_STR(sContact.c_str()),TAG_END());
				 */
		}

		XGLOG_INFO("$$$$$$$$$$$$$$$$$$$$$$ TRYING TO SEND 180 RINGING $$$$$$$$$$$$$$$$", pSipMgc->uid.c_str());

		nua_respond((nua_handle_t *)pSipMgc->sofianh, 180, "Ringing",
					TAG_IF(!pSipMgc->m_sAnsSdp.empty(), SIPTAG_CONTENT_TYPE_STR("application/sdp")),
					TAG_IF(!pSipMgc->m_sAnsSdp.empty(), SIPTAG_PAYLOAD_STR(pSipMgc->m_sAnsSdp.c_str())),
					SIPTAG_SUPPORTED_STR(""),
					TAG_IF(!pSipMgc->cp.m_sRespContact.empty(),SIPTAG_CONTACT_STR(pSipMgc->cp.m_sRespContact.c_str())),
					TAG_END());

		pSipMgc->callstate = EN_CS_ALERTING;

		break;
	}
	case XGATESIPEVENT::EN_183_REL:
	{
#if 1
		if (pSipMgc->m_sAnsSdp != "")
		{

			nua_set_hparams((nua_handle_t *)pSipMgc->sofianh, NUTAG_EARLY_MEDIA(1),
							NUTAG_EARLY_ANSWER(1),
							NUTAG_AUTOANSWER(0),
							TAG_NULL());
		}
		// nua_set_hparams((nua_handle_t*)pSipMgc->sofianh,SIPTAG_CONTACT_STR(strContact.c_str()));

		XGLOG_INFO("$$$$$$$$$$$$$$$$$$$$$$ TRYING TO SEND 183 PROCESSING $$$$$$$$$$$$$$$$", pSipMgc->uid.c_str());
		// SofiaRespond((nua_handle_t*)pSipMgc->sofianh,EN_183_REL,"Processing");
		nua_respond((nua_handle_t *)pSipMgc->sofianh, 183, "Processing",
					TAG_IF(!pSipMgc->m_sAnsSdp.empty(), SIPTAG_CONTENT_TYPE_STR("application/sdp")),
					TAG_IF(!pSipMgc->m_sAnsSdp.empty(), SIPTAG_PAYLOAD_STR(pSipMgc->m_sAnsSdp.c_str())),
					SIPTAG_SUPPORTED_STR(""),
					TAG_IF(!pSipMgc->cp.m_sRespContact.empty(),SIPTAG_CONTACT_STR(pSipMgc->cp.m_sRespContact.c_str())),
					TAG_END());
#endif
		break;
	}
	default:
		break;
	}
	XGLOG_INFO("SofiaSiphandler::RespondWith1xx Event(%d - %s) ", pSipMgc->sipeventtype, pSipMgc->uid.c_str());
	return xGateReturn::SUCCESS;
}

xGateReturn SofiaSiphandler::RespondWith2xx(xGateSofiaobj *pSipMgc)
{
	XGLOG_FUNCTION_EN_EX("");

	if (!pSipMgc)
	{
		XGLOG_ERROR("SofiaSiphandler::RespondWith2xx Invalid pSipMgc");
		return xGateReturn::FAIL;
	}

	sdp_session_t *rsdp = nullptr;

	switch (pSipMgc->sipeventtype)
	{
	case XGATESIPEVENT::EN_200_REL:

		// rsdp = (sdp_session_t*)pSipMgc->rsdp;
#if 1
		if (pSipMgc->m_sAnsSdp != "")
		{
			XGLOG_INFO("SofiaSiphandler::RespondWith2xx pSipMgc->rsdp is not empty for (%s)", pSipMgc->uid.c_str());
			/*set local SDP start*/

			nua_set_hparams((nua_handle_t *)pSipMgc->sofianh,
							// SOATAG_AUDIO_AUX("telephone-event"),
							// SOATAG_USER_SDP(rsdp),/*SOATAG_USER_SDP_STR(sSdp),*/
							TAG_END());

			nua_set_hparams((nua_handle_t *)pSipMgc->sofianh, NUTAG_EARLY_MEDIA(0),
							NUTAG_EARLY_ANSWER(1),
							NUTAG_AUTOANSWER(0),
							TAG_NULL());

			/*set local SDP ends*/

			std::string t_useragent("");
			sip_t *sip = (sip_t *)pSipMgc->siptag;

			std::string orgcallid("");
			if (pSipMgc->SofiaLegB->cp.isClientDialout == true) // To Avoid reading unknown header multiple times
			{
				if (pSipMgc && (sip_t *)pSipMgc->SofiaLegB && (sip_t *)pSipMgc->SofiaLegB->siptag)
				{
					sip_t *sip_remotepeer = (sip_t *)pSipMgc->SofiaLegB->siptag;
					if (sip_remotepeer && sip_remotepeer->sip_unknown && sip_remotepeer->sip_unknown->un_name &&
						sip_remotepeer->sip_unknown->un_value)
					{
						for (sip_unknown_t *h = sip_remotepeer->sip_unknown; h != nullptr; h = h->un_next)
						{
							if (strcmp(h->un_name, "Org-Call-ID") == 0)
							{
								orgcallid = "Org-Call-ID:";
								orgcallid += h->un_value;
								XGLOG_INFO("orgcallid =%s", orgcallid.c_str());
								cout << "Org-Call-ID:" << orgcallid.c_str() << endl;
							}
						}
					}
				}
			}

			nua_respond((nua_handle_t *)pSipMgc->sofianh, 200, "OK",
						TAG_IF(!pSipMgc->cp.calledDeviceType.empty(), SIPTAG_USER_AGENT_STR(pSipMgc->cp.calledDeviceType.c_str())),
						SIPTAG_CONTENT_TYPE_STR("application/sdp"),
						SIPTAG_PAYLOAD_STR(pSipMgc->m_sAnsSdp.c_str()),
						SIPTAG_USER_AGENT_STR(t_useragent.c_str()),
						SIPTAG_HEADER_STR(orgcallid.c_str()),
						SIPTAG_SUPPORTED_STR(""),
						TAG_IF(!pSipMgc->cp.m_sRespContact.empty(),SIPTAG_CONTACT_STR(pSipMgc->cp.m_sRespContact.c_str())),
						TAG_NULL());

			pSipMgc->SofiaLegB->cp.isClientDialout = false;
			pSipMgc->SofiaLegB->callstate = EN_CS_CONNECTED;
			pSipMgc->callstate = EN_CS_CONNECTED;
			pSipMgc->SofiaLegB->mediastate = EN_MED_CONNECTED;
		}
		else
		{
			XGLOG_INFO("SofiaSiphandler::RespondWith2xx Media create failed %s", pSipMgc->uid.c_str());
			RespondToPeer(EN_503_REL, pSipMgc);
		}
#endif
		break;

	default:
		break;
	}
	XGLOG_INFO("SofiaSiphandler::RespondWith2xx Event(%d - %s) ", pSipMgc->sipeventtype, pSipMgc->uid.c_str());
	return xGateReturn::SUCCESS;
}

/*
 */
xGateReturn SofiaSiphandler::RespondWith3xx(xGateSofiaobj *pSipMgc)
{
	switch (pSipMgc->sipeventtype)
	{
	case XGATESIPEVENT::EN_300_REL:
		break;
	default:
		break;
	}
	XGLOG_INFO("SofiaSiphandler::RespondWith3xx Event(%d - %s) ", pSipMgc->sipeventtype, pSipMgc->callid.c_str());
	return xGateReturn::SUCCESS;
}
/*
 */
xGateReturn SofiaSiphandler::RespondWith4xx(xGateSofiaobj *pSipMgc)
{
	XGLOG_INFO("SofiaSiphandler::RespondWith4xx()....entered ");

	if (!pSipMgc && !pSipMgc->sofianh)
	{
		XGLOG_INFO("SofiaSiphandler::RespondWith4xx - Invalid sofia handle");
		return xGateReturn::FAIL;
	}

	switch (pSipMgc->sipeventtype)
	{
	case XGATESIPEVENT::EN_400_REL:
		SofiaRespond((nua_handle_t *)pSipMgc->sofianh, EN_400_REL, "Bad Request");
		break;
	case XGATESIPEVENT::EN_401_REL:
	{
	}
		return xGateReturn::SUCCESS;
	case XGATESIPEVENT::EN_407_REL:
	{
	}
		return xGateReturn::SUCCESS;
	case XGATESIPEVENT::EN_403_REL:
		SofiaRespond((nua_handle_t *)pSipMgc->sofianh, EN_403_REL, "Forbidden");
		break;
	case XGATESIPEVENT::EN_404_REL:
		SofiaRespond((nua_handle_t *)pSipMgc->sofianh, EN_404_REL, "Not Found  or No Route");
		break;
	case XGATESIPEVENT::EN_408_REL:
		SofiaRespond((nua_handle_t *)pSipMgc->sofianh, EN_408_REL, "Request Timeout");
		break;
	case XGATESIPEVENT::EN_410_REL:
		SofiaRespond((nua_handle_t *)pSipMgc->sofianh, EN_410_REL, "Gone");
		break;
	case XGATESIPEVENT::EN_480_REL:
		SofiaRespond((nua_handle_t *)pSipMgc->sofianh, EN_480_REL, "Temporarily Unavailable");
		break;
	case XGATESIPEVENT::EN_482_REL:
		SofiaRespond((nua_handle_t *)pSipMgc->sofianh, EN_482_REL, "Loop Detected");
		break;
	case XGATESIPEVENT::EN_484_REL:
		SofiaRespond((nua_handle_t *)pSipMgc->sofianh, EN_484_REL, "Address Incomplete");
		break;
	case XGATESIPEVENT::EN_486_REL:
		SofiaRespond((nua_handle_t *)pSipMgc->sofianh, EN_486_REL, "Busy Here");
		break;
	case XGATESIPEVENT::EN_487_REL:
		SofiaRespond((nua_handle_t *)pSipMgc->sofianh, EN_487_REL, "Request Terminated");
		break;
	case XGATESIPEVENT::EN_488_REL:
		SofiaRespond((nua_handle_t *)pSipMgc->sofianh, EN_488_REL, "Not Acceptable Here");
		break;
	default:
		SofiaRespond((nua_handle_t *)pSipMgc->sofianh, EN_408_REL, "default-Request Timeout");
		break;
	}

	return xGateReturn::SUCCESS;
}
/*
 */
xGateReturn SofiaSiphandler::RespondWith5xx(xGateSofiaobj *pSipMgc)
{
	XGLOG_INFO("SofiaSiphandler::RespondWith5xx()....entered ");
	if (!pSipMgc && !pSipMgc->sofianh)
	{
		XGLOG_INFO("SofiaSiphandler::RespondWith4xx - Invalid sofia handle");
		return xGateReturn::FAIL;
	}

	XGLOG_INFO("SofiaSiphandler::RespondWith5xx Event(%d - %s) ", pSipMgc->sipeventtype, pSipMgc->uid.c_str());
	switch (pSipMgc->sipeventtype)
	{
	case XGATESIPEVENT::EN_500_REL:
		SofiaRespond((nua_handle_t *)pSipMgc->sofianh, EN_500_REL, "Server Internal Error");
		XGLOG_INFO("SofiaSiphandler::RespondWith5xx Event(%d - %s) ", pSipMgc->sipeventtype, pSipMgc->uid.c_str());
		break;
	case XGATESIPEVENT::EN_501_REL:
		SofiaRespond((nua_handle_t *)pSipMgc->sofianh, EN_501_REL, "Not Implemented");
		break;
	case XGATESIPEVENT::EN_502_REL:
		SofiaRespond((nua_handle_t *)pSipMgc->sofianh, EN_502_REL, "Bad Gateway");
		break;
	case XGATESIPEVENT::EN_503_REL:
		SofiaRespond((nua_handle_t *)pSipMgc->sofianh, EN_503_REL, "Service Unavailable");
		break;
	case XGATESIPEVENT::EN_504_REL:
		SofiaRespond((nua_handle_t *)pSipMgc->sofianh, EN_504_REL, "Server Time-out");
		break;
	case XGATESIPEVENT::EN_510_REL:
		SofiaRespond((nua_handle_t *)pSipMgc->sofianh, EN_510_REL, "Facility Rejected");
		break;
	default:
		SofiaRespond((nua_handle_t *)pSipMgc->sofianh, EN_504_REL, "default-Server Time-out");
		break;
	}
	// since it is error in Server we need to delete the call
	/*if(pSipMgc->SofiaLegB && pSipMgc->SofiaLegB->sofianh)
		SofiaSendBye((nua_handle_t*)pSipMgc->SofiaLegB->sofianh);*/

	// DeleteCallInfo((pSipMgc));

	return xGateReturn::SUCCESS;
}
/*
 */
xGateReturn SofiaSiphandler::RespondWith6xx(xGateSofiaobj *pSipMgc)
{

	XGLOG_INFO("SofiaSiphandler::RespondWith6xx()....entered ");
	XGLOG_INFO("SofiaSiphandler::RespondWith6xx Event(%d - %s) ", pSipMgc->sipeventtype, pSipMgc->uid.c_str());
	switch (pSipMgc->sipeventtype)
	{
	case XGATESIPEVENT::EN_600_REL:
		SofiaRespond((nua_handle_t *)pSipMgc->sofianh, EN_600_REL, "Busy Everywhere");
		break;
	case XGATESIPEVENT::EN_603_REL:
		SofiaRespond((nua_handle_t *)pSipMgc->sofianh, EN_603_REL, "Decline");
		break;
	case XGATESIPEVENT::EN_604_REL:
		SofiaRespond((nua_handle_t *)pSipMgc->sofianh, EN_604_REL, "Does Not Exist Anywhere");
		break;
	case XGATESIPEVENT::EN_606_REL:
		SofiaRespond((nua_handle_t *)pSipMgc->sofianh, EN_606_REL, "Not Acceptable");
		break;
	case XGATESIPEVENT::EN_607_REL:
		SofiaRespond((nua_handle_t *)pSipMgc->sofianh, EN_607_REL, "Unwanted");
		break;
	default:
		XGLOG_INFO("SofiaSiphandler::RespondWith6xx Event(%d)- Event Not Impemented", pSipMgc->sipeventtype);
		break;
	}

	return xGateReturn::SUCCESS;
}

xGateReturn SofiaSiphandler::RegistarResponse(xGateSofiaSipRegobj *regobj)
{
	XGLOG_FUNCTION_EN_EX("");
	XGLOG_INFO("RegistarReponse(%s)", regobj->callid.c_str());
	xGateReturn ret = SUCCESS;
	sip_t *sip = (sip_t *)regobj->regsip;
	nua_handle_t *nh = (nua_handle_t *)regobj->Reghandle;
	switch (regobj->Regevent)
	{
	case EN_VALIDATE:
	{
		char contact_uri[500];
		sprintf(contact_uri, "<sip:%s@%s:%s;%s>", sip->sip_from->a_url->url_user, sip->sip_contact->m_url->url_host,
				sip->sip_contact->m_url->url_port, sip->sip_contact->m_url->url_params);
		nua_respond(nh, SIP_100_TRYING, SIPTAG_CONTACT_STR(contact_uri), SIPTAG_SUPPORTED_STR(""),
					SIPTAG_DATE(sip->sip_date), NUTAG_WITH(nua_saved_event_request((nua_saved_event_t const *)regobj->sofiahmsg)),
					NUTAG_M_PARAMS(sip->sip_contact->m_url->url_params), TAG_END());
	}
	break;
	case EN_GETUSRPWD:
	{
		XGLOG_INFO("RegistarReponse(%s)-- Get User Name Password for verification", regobj->callid.c_str());
		break;
	}
	case EN_REGISTER:
	case EN_DEREGISTER:
	{
		// char reg[15] = "REGISTER";
		std::string reg = "REGISTER";
		if (VerifyAuthorisationResponce(sip, regobj->password, reg) == false)
		{
			ret = FAIL;
			XGLOG_INFO("SofiaSiphandler", "RegistarReponse(%s)-- Auth Failed", regobj->callid.c_str());
		}
		else
		{
			XGLOG_INFO("SofiaSiphandler", "RegistarReponse(%s)-- VerifyAuthorisationResponce Success", regobj->callid.c_str());
			// On Success verfication send 200 ok
			xGateCallObj *callobj = new xGateCallObj;
			callobj->sipeventtype = EN_REGISTRATION_REQ;
			if ((regobj->expries) > 0)
			{
				regobj->Regevent = EN_REGISTER;
			}
			else
			{
				regobj->Regevent = EN_DEREGISTER;
			}
			callobj->m_regObj = regobj;
			PostMessage(callobj, XGATEPOSTTYPE::EN_PROCESS);
			break; // Break is required only success case
		}
		break;
	}
	case EN_REG_FAILED:
	case EN_SEND401:
	{
		char authwww[624] = "";
		char buffer[512] = "";
		mod = SofiaInitAuth(m_root);
		if (mod)
		{

			auth_generate_digest_nonce(mod, buffer, 512, false, msg_now());

			snprintf(authwww,624," Digest realm=\"%s\",nonce=\"%s\"",
					sip->sip_to->a_url->url_host, buffer);

			XGLOG_INFO("SofiaSiphandler RegistarReponse(%s)-- send 401 unauthorized response", regobj->callid.c_str());
			nua_respond(nh, 401, "Unauthorized", SIPTAG_WWW_AUTHENTICATE_STR(authwww), SIPTAG_SUPPORTED_STR(""),
						SIPTAG_DATE(sip->sip_date), NUTAG_WITH_THIS(m_nuaHandle), NUTAG_WITH_SAVED((nua_saved_event_t const *)regobj->sofiahmsg),
						NUTAG_M_PARAMS(sip->sip_contact->m_url->url_params), TAG_END());
			nua_destroy_event((nua_saved_event_t *)regobj->sofiahmsg);
		}
	}
		if (regobj)
			delete regobj;
		break;

	case EN_REG_SUCESS:
	{
		XGLOG_INFO("RegistarReponse --> EN_REG_SUCESS for callid:%s", regobj->callid.c_str());
		/*AOR Cache Requirement Start*/
		ClientAOR *ptClientAOR = nullptr;

		map<std::string, ClientAOR *>::iterator itr;
		itr = this->m_mapConfUserAor.find(regobj->URAVConfConnID);

		if (itr == this->m_mapConfUserAor.end())
		{
			ptClientAOR = new ClientAOR();
		}
		else
		{
			ptClientAOR = itr->second;
		}

		ptClientAOR->m_contactAddr = regobj->contactaddress;
		ptClientAOR->m_meetingID = regobj->URAVConfRoomID;
		ptClientAOR->m_transport = regobj->transporttype;

		this->m_mapConfUserAor[regobj->URAVConfConnID] = ptClientAOR;

		/*AOR Cache Requirement End*/
		sip_contact_t *head = nullptr, *loop = nullptr;

		sip_contact_t *ctemp = nullptr;

		std::string aorRecords = regobj->Addressofrecords.c_str();
		std::string delimiter = "#";

		int first = 0;

		size_t pos = 0;
		std::string token;
		while ((pos = aorRecords.find(delimiter)) != std::string::npos)
		{
			token = aorRecords.substr(0, pos);
			std::cout << token << std::endl;
			if (first == 0)
			{
				head = sip_contact_make(&m_homeMemory, token.c_str());
				if (head == nullptr)
				{
					return FAIL;
				}
				head->m_next = nullptr;
				loop = head;
			}
			else
			{
				ctemp = sip_contact_make(&m_homeMemory, token.c_str());
				if (ctemp == nullptr)
				{
					return FAIL;
				}
				ctemp->m_next = nullptr;
				loop->m_next = ctemp;
				loop = ctemp;
			}

			first++;
			aorRecords.erase(0, pos + delimiter.length());
		}
		nua_respond(nh, 200, "OK", SIPTAG_CONTACT(head), SIPTAG_DATE(sip->sip_date), SIPTAG_SUPPORTED_STR(""),
					NUTAG_WITH_THIS(m_nuaHandle), NUTAG_WITH_SAVED((nua_saved_event_t const *)regobj->sofiahmsg),
					NUTAG_M_PARAMS(sip->sip_contact->m_url->url_params), TAG_END());
		nua_destroy_event((nua_saved_event_t *)regobj->sofiahmsg);
		SofiaSendMessageRequest(regobj);
	}

		if (regobj)
			delete regobj;
		break;
	case EN_AUTHCALL:
	{ // Event will handle invite auth
		if (VerifyAuthorisationResponce(sip, regobj->password, "INVITE") == false)
		{
			XGLOG_INFO("RegistarReponse(%s)- Call/Session Auth Verification failed", regobj->callid.c_str());
			ret = FAIL;
		}
		else
		{

			std::string callidstring = CreateUniqueCallid(sip->sip_from->a_url->url_user,
														  sip->sip_from->a_url->url_host,
														  sip->sip_from->a_tag,
														  sip->sip_call_id->i_id);

			/*LWSS Changes Start*/
			// sobj->callauthstate = 1;
			XGLOG_INFO("RegistarReponse(%s)- Call/Session Verification Successed", callidstring.c_str());
			ProcessWssInviteRequest((nua_handle_t *)regobj->Reghandle, sip, (tagi_t *)regobj->siptags, callidstring, 1);

			if (regobj)
				delete regobj;
			break;
		}
	}
	case EN_SEND407:
	{
		char proxyauth[624] = "";
		char buffer[512] = "";
		mod = SofiaInitAuth(m_root);
		if (mod)
		{

			auth_generate_digest_nonce(mod, buffer, 512, false, msg_now());
			snprintf(proxyauth,624," Digest realm=\"%s\",nonce=\"%s\"",
					sip->sip_contact->m_url->url_host, buffer);

			nua_respond(nh, 407, "Proxy Authentication Required", SIPTAG_PROXY_AUTHENTICATE_STR(proxyauth),
						SIPTAG_SUPPORTED_STR(""), SIPTAG_DATE(sip->sip_date), TAG_END());
			XGLOG_INFO("SofiaSiphandler", "RegistarReponse(%s)- 407 Proxy Authentication Required sent", regobj->callid.c_str());
		}
		if (regobj)
			delete regobj;
		break;
	}
	case EN_REG_RESPONSE:
	{

		handleRegResponse(regobj);
		break;
	}
	case EN_REG_RESPONSE_ERROR:
	{
		handleRegResponseError(regobj);
		break;
	}
	default:
		break;
	}
	return ret;
}

void SofiaSiphandler::SendRegister()
{
	SipModuleProfile sipProf = URENGINE_CONFIG::instance()->getSipModConfig();
	// SipModuleProfile sipProf = IVR_CONFIG::instance()->getSipModConfig();

	xGateSofiaSipRegobj *sipregobj = new xGateSofiaSipRegobj();
	nua_handle_t *reg_handle;
	XGLOG_INFO("SofiaSiphandler::SendRegister Entering");
#if 0
	reg_handle = nua_handle(GetAppContext(),
			NULL,NUTAG_REGISTRAR("sip:6665.UR.mundio.com:5060"), SIPTAG_TO_STR("sip:200@6665.UR.mundio.com:5060"),
			SIPTAG_FROM_STR("sip:200@6665.UR.mundio.com:5060"),
			SIPTAG_CONTACT_STR("sip:200-lb7abo31n5rf1@10.22.5.39:5060;transport=udp"),
			TAG_END());
#endif
	reg_handle = nua_handle(GetAppContext(),
							NULL, NUTAG_REGISTRAR(sipProf.registerReqUri.c_str()), SIPTAG_TO_STR(sipProf.registerToUri.c_str()),
							SIPTAG_FROM_STR(sipProf.registerToUri.c_str()),
							// SIPTAG_CONTACT_STR(sipProf.registerContactUri.c_str()),
							TAG_END());

	sipregobj->Reghandle = reg_handle;

	nua_register(reg_handle, TAG_END());
	XGLOG_INFO("SofiaSiphandler::SendRegister Exiting");
}

void SofiaSiphandler::SendDeRegister(char *to_uri, char *from_uri, char *contact_uri)
{
	XGLOG_INFO("SofiaSiphandler::SendDeRegister Entering");
	nua_handle_t *reg_handle;
	reg_handle = nua_handle(GetAppContext(),
							NULL, NUTAG_REGISTRAR("sip:6665.UR.mundio.com:5060"), SIPTAG_TO_STR("sip:200@6665.UR.mundio.com:5060"),
							SIPTAG_FROM_STR("sip:200@6665.UR.mundio.com:5060"),
							SIPTAG_CONTACT_STR("*;expires=0"),
							TAG_END());

	XGLOG_INFO("SofiaSiphandler::SendDeRegister Exiting");
}

void SofiaSiphandler::SendRefreshRegister(char *to_uri, char *from_uri, char *contact_uri)
{
	XGLOG_INFO("SofiaSiphandler::SendRefreshRegister Entering");

	XGLOG_INFO("SofiaSiphandler::SendRefreshRegister Exiting");
}

bool SofiaSiphandler::ValidateSipMsgPointer(const char *p)
{
	if (p != nullptr)
	{
		if (strlen(p))
			return true;
	}
	return false;
}

/*
* handle Register Response from controller 
*/
bool SofiaSiphandler::handleRegResponse(xGateSofiaSipRegobj *regobj)
{
	if (!regobj)
	{
		XGLOG_ERROR("SofiaSiphandler::handleRegResponse regobj is nullptr");
		return false;
	}
	// Todo:: Siva below logic should rewrite properly
	sip_t *sip = (sip_t *)regobj->regsip;
	nua_handle_t *nh = (nua_handle_t *)regobj->Reghandle;
	std::string reg = "REGISTER";
	if (VerifyAuthorisationResponce(sip, regobj->password, reg) == false)
	{
		nua_respond(nh, 403, "Forbidden", NUTAG_WITH_SAVED((nua_saved_event_t const *)regobj->sofiahmsg), TAG_END());
		xGateCallObj *callobj = new xGateCallObj;
		regobj->Regevent = EN_REG_UPDATE_AUTH_FAILED;
		callobj->m_regObj = regobj;
		callobj->sipeventtype = EN_REGISTRATION_REQ;
		PostMessage(callobj, XGATEPOSTTYPE::EN_PROCESS);
		DestroySavedEvent((nua_saved_event_t *)regobj->sofiahmsg);
		SofiaHandleDestroy(nh);
	}
	else
	{
		/*AOR Cache Requirement Start*/
		ClientAOR *ptClientAOR = nullptr;

		map<std::string, ClientAOR *>::iterator itr;
		itr = this->m_mapConfUserAor.find(regobj->URAVConfConnID);

		if (itr == this->m_mapConfUserAor.end())
		{
			ptClientAOR = new ClientAOR();
		}
		else
		{
			ptClientAOR = itr->second;
		}

		ptClientAOR->m_contactAddr = regobj->contactaddress;
		ptClientAOR->m_meetingID = regobj->URAVConfRoomID;
		ptClientAOR->m_transport = regobj->transporttype;

		this->m_mapConfUserAor[regobj->URAVConfConnID] = ptClientAOR;

		/*AOR Cache Requirement End*/
		std::string contact("");
		sip_contact_t *ptcontact = nullptr;
		if(regobj->expries!=0)
		   contact = std::move(generate_reg_contact(regobj));
		if(!contact.empty())
			ptcontact = sip_contact_make(&m_homeMemory, contact.c_str());
		nua_respond(nh, 200, "OK", TAG_IF(ptcontact,SIPTAG_CONTACT(ptcontact)), SIPTAG_DATE(sip->sip_date), SIPTAG_SUPPORTED_STR(""),
					NUTAG_WITH_THIS(m_nuaHandle), NUTAG_WITH_SAVED((nua_saved_event_t const *)regobj->sofiahmsg),
					NUTAG_M_PARAMS(sip->sip_contact->m_url->url_params), TAG_END());
		XGLOG_TRACE("Registration_from(%s), 200 OK Successfully send to the client ,domain(%s) device(%s) callid(%s)" ,sip->sip_from->a_url->url_user,\
	                                                                        										  sip->sip_from->a_url->url_host,\
														                    										  (sip->sip_user_agent && sip->sip_user_agent->g_string)?sip->sip_user_agent->g_string:"device_empty",\
																	        										  sip->sip_call_id->i_id);			
		DestroySavedEvent((nua_saved_event_t *)regobj->sofiahmsg);
		SofiaHandleDestroy(nh);
		SofiaSendMessageRequest(regobj);
		// Sending Update Request to DB (Interagator)
		xGateCallObj *callobj = new xGateCallObj;
		regobj->Regevent = EN_REG_UPDATE_AUTH_SUCCESS;
		callobj->m_regObj = regobj;
		callobj->sipeventtype = EN_REGISTRATION_REQ;
		PostMessage(callobj, XGATEPOSTTYPE::EN_PROCESS);
	}

	return true;
}

/*
 * handle Register Response error from controller
 */
bool SofiaSiphandler::handleRegResponseError(xGateSofiaSipRegobj *regobj)
{
	if (!regobj)
	{
		XGLOG_ERROR("SofiaSiphandler::handleRegResponseError regobj is nullptr");
		return false;
	}
	sip_t *sip = (sip_t *)regobj->regsip;
	nua_handle_t *nh = (nua_handle_t *)regobj->Reghandle;

	nua_respond(nh, 403, "Forbidden", SIPTAG_SUPPORTED_STR(""),
				NUTAG_WITH_THIS(m_nuaHandle), NUTAG_WITH_SAVED((nua_saved_event_t const *)regobj->sofiahmsg),
				NUTAG_M_PARAMS(sip->sip_contact->m_url->url_params), TAG_END());
	XGLOG_ERROR("SofiaSiphandler::handleRegResponseError sending 403 forbidden to client ");

	SofiaHandleDestroy(nh);
	DestroySavedEvent((nua_saved_event_t *)regobj->sofiahmsg);
	delete regobj;
	return true;
}

void SofiaSiphandler::SofiaInit()
{
	XGLOG_INFO("SofiaSiphandler::SofiaInit()");
	su_init();
}

/* Used to De - initialize run-time environment including sockets for Sofia Library
 * Input Param : Void
 */
void SofiaSiphandler::SofiaDeInit()
{
	su_deinit();
}

/* Used to initialize applicaton memory for Sofia Library,
 * This helps to start mulitple sofia library using differnt memeory
 * Input Param : su_home_t structure address
 */
void SofiaSiphandler::SofiaInitMemory(su_home_t &homeMemory)
{
	su_home_init(&homeMemory);
}

/* Used to deinitialize applicaton memory for Sofia Library,
 *  Input Param : su_home_t structure address
 */
void SofiaSiphandler::SofiaDeInitMemory(su_home_t &homeMemory)
{
	su_home_deinit(&homeMemory);
}

/*
* Process events, timers and messages.
*/
void SofiaSiphandler::SofiaRootStep(nua_t *sua_handle, su_root_t *nuaRoot)
{
	if (sua_handle)
	  su_root_step(nuaRoot, 0);
}

/*
* Run event and message loop.
*/
void SofiaSiphandler::SofiaRootRun(su_root_t *nuaRoot)
{
	if(nuaRoot)
		su_root_run(nuaRoot);	
}

/* Used to create root instance
 */
su_root_t *SofiaSiphandler::SofiaInitRoot(su_root_magic_t *appInitance)
{
	return su_root_create(appInitance);
}

/* Used to destroy root instance
 */
void SofiaSiphandler::SofiaDeInitRoot(su_root_t *nuaRoot)
{
	su_root_destroy(nuaRoot);
}

/*
 * Create auth mod for authendicating client
 * Still need to chagen the paramater for auth registration
 * */

auth_mod_t *SofiaSiphandler::SofiaInitAuth(su_root_t *root)
{
	return auth_mod_create(root, AUTHTAG_METHOD("Digest"), AUTHTAG_ALGORITHM("MD5"), AUTHTAG_QOP("auth"), AUTHTAG_REALM("xgatesipreg.vectone.com"), TAG_END());
}
void SofiaSiphandler::SofiaDeInitAuth(auth_mod_t *authmod)
{
	if (authmod)
		auth_mod_destroy(authmod);
}

/**/
nua_t *SofiaSiphandler::SofiaCreate(su_root_t *root, nua_callback_f callback, nua_magic_t *magic)
{

	/* @NEW_1_12_7. In order to use @b Alert-Info header, initialize the SIP
	 * parser before calling nta_agent_create() or nua_create() with, e.g.,
	 * sip_update_default_mclass(sip_extend_mclass(nullptr)). Sofia -Sip Lib
	 */
	sip_update_default_mclass(sip_extend_mclass(nullptr));

	m_nuaHandle = nua_create(root, callback, magic,
							 NUTAG_URL(m_pSipHndlrParams->m_sSipURL.c_str()),
							 NUTAG_WSS_URL(m_pSipHndlrParams->m_sWssURL.c_str()),
							 NUTAG_SIPS_URL(m_pSipHndlrParams->m_sSipsURL.c_str()),
							 NUTAG_CERTIFICATE_DIR(m_pSipHndlrParams->m_sCertPath.c_str()),
							 TAG_END());

	SofiaSetParam(m_nuaHandle);
	return m_nuaHandle;
}

void SofiaSiphandler::SofiaSetParam(nua_t *nuaHandle)
{
	nua_set_params(nuaHandle, SETPARAM);
}

void SofiaSiphandler::SofiaShutdown(nua_t *nuaHandle)
{
	nua_shutdown(nuaHandle);
}

void SofiaSiphandler::SofiaSetLog(int level)
{
	// su_log_t* sulog =  new su_log_t;
	// su_log_set_level(su_log_global,level); //windows call
	// su_log_redirect(nullptr,&SofiaSiphandler::SofiaLoggerCallback,nullptr);//to enable stack logs into application logs
}

void SofiaSiphandler::SofiaHandleDestroy(nua_handle_t *nh)
{
	nua_handle_destroy(nh);
}

/**/
void SofiaSiphandler::SofiaDestory(nua_t *nuaHandle)
{
	nua_destroy(nuaHandle);
}

/**/
void SofiaSiphandler::SofiaRespond(nua_handle_t *nh, int status, char const *phrase)
{
	nua_respond(nh, status, phrase, TAG_END());
}

/**/
void SofiaSiphandler::SofiaRespond(nua_handle_t *nh, int status, char const *phrase, tagi_t tags[])
{
	// nua_respond(nh,status,phrase,tags);
}

/**/
void SofiaSiphandler::SofiaRespond(nua_handle_t *nh, int status, char const *phrase, nua_saved_event_t const *savedEvent)
{
	nua_respond(nh, status, phrase, NUTAG_WITH(nua_saved_event_request(savedEvent)), SIPTAG_SUPPORTED_STR(""), TAG_END());
	// nua_destroy_event(&savedEvent);
}

/**/
void SofiaSiphandler::SofiaRespondWithSavedEvent(nua_handle_t *nh, int status, char const *phrase)
{
	nua_saved_event_t *sEvent = CreateSavedEvent();
	nua_respond(nh, status, phrase, NUTAG_WITH(nua_saved_event_request(sEvent)), TAG_END());
	DestroySavedEvent(sEvent);
}

/*
 * Abstract type for saved nua events.
 * Save last nua event.
 */
nua_saved_event_t *SofiaSiphandler::CreateSavedEvent()
{
	nua_saved_event_t *pSavedEvent = new nua_saved_event_t;
	nua_save_event(m_nuaHandle, pSavedEvent);
	return pSavedEvent;
}

/*
 *Destroy the saved events
 */
void SofiaSiphandler::DestroySavedEvent(nua_saved_event_t *saved)
{
	nua_destroy_event(saved);
	delete saved;
}

/**/
void SofiaSiphandler::SofiaSendAck(nua_handle_t *nh)
{
	nua_ack(nh, TAG_END());
}

/**/
void SofiaSiphandler::SofiaSendInvite(nua_handle_t *nh)
{
	nua_invite(nh, TAG_END());
}

/**/
void SofiaSiphandler::SofiaSendBye(nua_handle_t *nh)
{
	nua_bye(nh, TAG_END());
}

void SofiaSiphandler::auth_MD5(nua_handle_t *nh, sip_t const *sip)
{
	SipModuleProfile sipProf = URENGINE_CONFIG::instance()->getSipModConfig();

	sip_www_authenticate_t const *wa = sip->sip_www_authenticate;
	sip_proxy_authenticate_t const *pa = sip->sip_proxy_authenticate;
	const char *method = nullptr;
	const char *realm = nullptr;

	char auth[100] = "";

	if (wa)
	{
		realm = msg_params_find(wa->au_params, "realm=");
		method = wa->au_scheme;
		XGLOG_INFO("Sofia_Debug :>>>>>>>>>>>>>> sip >>>>> ***2 realm auth_MD5 wa auth is");
	}

	if (pa)
	{
		realm = msg_params_find(pa->au_params, "realm=");
		method = pa->au_scheme;
		XGLOG_INFO("Sofia_Debug :>>>>>>>>>>>>>> sip >>>>> ***2 realm auth_MD5 proxy auth");
	}

	if (realm == nullptr)
	{
		return;
	}

	// char *username = deblank(wAccount.user);
	// char *passwd = deblank(wAccount.password);
	// char *username = "200"; //deblank(wAccount.user);
	// char *passwd = "NXXOSG73ZY30CAM"; //deblank(wAccount.password);
	char *username = deblank((char *)sipProf.registerUsername.c_str());
	char *passwd = deblank((char *)sipProf.registerPassword.c_str());

	sprintf(auth, "%s:%s:%s:%s",
			method, realm, (const char *)username, (const char *)passwd);
	nua_authenticate(nh, NUTAG_AUTH(auth), TAG_END());
}

void SofiaSiphandler::SofiaBindhandle(nua_handle_t *nh, nua_hmagic_t *sofiaobjhandle)
{
	nua_handle_bind(nh, sofiaobjhandle);
}

url_t *SofiaSiphandler::SofiaCreateMake(su_home_t &su_home, const char *dialstr)
{

	SofiaInitMemory(su_home);
	return url_make(&su_home, dialstr);
}

// Siva::Implemented below fucntion to dump  stack logs into Application logger file
void SofiaSiphandler::SofiaLoggerCallback(void *stream, char const *fmt, va_list ap)
{
	char message[2048];
	vsnprintf(message, sizeof(message), fmt, ap);

	if (strcmp(message, "") && strcmp(message, "\n"))
	{
		if ((strstr(message, "events IN HUP") != nullptr) || (strstr(message, "nta_agent: tport: Invalid argument") != nullptr))
		{
		}
		else
		{
			XGLOG_INFO("SofiaStack %s", message);
		}
	}
}

void SofiaSiphandler::GetProcessRegisterEvent()
{
	xGateSofiaSipRegobj *RegQEvt = nullptr;
	if ((RegQEvt = PopFromReceiver()) != nullptr)
	{
		if (RegQEvt && (SofiaSiphandler *)RegQEvt->sofiamh)
		{
			((SofiaSiphandler *)RegQEvt->sofiamh)->RegistarResponse(RegQEvt);
		}
		else
		{
			XGLOG_ERROR("SofiaSiphandler::GetProcessRegisterEvent RegQEvt or RegQEvt->sofiamh is nullptr");
		}
	}
}

void SofiaSiphandler::GetProcessEvent()
{
	xGateSofiaobj *sobj = nullptr;
	xGateCallObj *callObj = nullptr;
	xGateCallPair *callpair = nullptr;

	try
	{
		if ((callObj = ReadFromReceiver()) != nullptr)
		{
			if( EN_OPTIONS_REQUEST ==  callObj->sipeventtype)
			{
			  	OnProcessEvent(callObj);
			}
			if (callpair = GetConnectedList(callObj->uid))
				sobj = callpair->Orgcall;
			if (sobj && (SofiaSiphandler *)sobj->sofiamh)
			{
				((SofiaSiphandler *)sobj->sofiamh)->OnProcessEvent(sobj, callObj);
				XGLOG_WARN("SofiaSiphandler", "GetProcessEvent  %s", callObj->uid.c_str());
			}
			else
			{
				XGLOG_INFO("SofiaSiphandler::GetProcessEvent sobj or sofiamh is nullptr :%s", callObj->uid.c_str());
			}
			if (callObj)
				delete callObj;
		}
	}
	catch (...)
	{
		XGLOG_WARN("SofiaSiphandler", "SetCallState- Exception ");
		return;
	}
}

void SofiaSiphandler::OnProcessEvent(xGateSofiaobj *sobj, xGateCallObj *callObj)
{

	if (!sobj || !callObj)
	{
		XGLOG_ERROR("SofiaSiphandler::OnProcessEvent callObj or sobj is nullptr");
		return;
	}
	switch (callObj->sipeventtype)
	{
	case EN_INVITE_RES:
	{
		handleInviteRespFromCtrl(sobj,callObj);
		break;
	}
	case EN_INVITE_INBOUND_PSTN_RES:
	{
		handleInviteInboundPstnRespFromCtrl(sobj,callObj);
		break;
	}
	case EN_INVITE_MS_TEAMS_RES:
	{
		handleInviteMSTeamsRespFromCtrl(sobj,callObj);
		break;
	}
	case EN_INVITE_RES_ERROR:
	{
		handleInviteRespErrFromCtrl(sobj,callObj);
		break;
	}
	case EN_INVITE_TIMEOUT:
	{
		XGLOG_INFO("SofiaSiphandler::OnProcessEvent cancel timer trigerd for uid:%s", callObj->uid.c_str());
		sobj->isInviteTimerStarted = false;
		timeout_invite_timer(sobj);
		break;
	}
	case EN_INVITE_INBOUND_PSTN_ERROR :
	{
        handleInviteInboundPstnRespErrFromCtrl(sobj,callObj);
		break;
	}
	default:
	{
		XGLOG_ERROR("SofiaSiphandler::OnProcessEvent unknown sipeventtype ! uid:%s", callObj->uid.c_str());
		break;
	}
	}
}

void SofiaSiphandler::OnProcessEvent(xGateCallObj *callObj)
{
	if (!callObj)
	{
		XGLOG_ERROR("SofiaSiphandler::OnProcessEvent callObj is nullptr");
		return;
	}

	switch (callObj->sipeventtype)
	{
	case EN_OPTIONS_REQUEST:
	{
		handleOptionsRequestFromMonitorService(callObj);
		break;
	}
	default:
	{
		XGLOG_ERROR("SofiaSiphandler::OnProcessEvent unknown sipeventtype !");
		break;
	}
	}
}

void SofiaSiphandler::SofiaCallBack(nua_event_t event,
									int status,
									char const *phrase,
									nua_t *nua,
									nua_magic_t *magic,
									nua_handle_t *nh,
									nua_hmagic_t *hmagic,
									sip_t const *sip,
									tagi_t tags[])
{
	XGLOG_INFO("SofiaSiphandler::SofiaCallBack(Event: %s status: %d Des: %s)", nua_event_name(event), status, phrase);
	printf("SofiaSiphandler::SofiaCallBack(Event: %s status: %d Des: %s\n)", nua_event_name(event), status, phrase);

	switch (event)
	{
	// Error & Status Handling Event Function
	case nua_i_active:
		XGLOG_INFO("SofiaSiphandler::SofiaCallBack(Event:nua_i_active)");
		break;
	case nua_i_error:
		XGLOG_INFO("SofiaSiphandler::SofiaCallBack(Event: %s status: %d Des: %s)", nua_event_name(event), status, phrase);
		break;
	case nua_i_fork:
		XGLOG_INFO("SofiaSiphandler::SofiaCallBack(Event:nua_i_fork)");
		break;
	case nua_i_media_error:
		XGLOG_INFO("SofiaSiphandler::SofiaCallBack(Event:nua_i_media_error)");
		break;
	case nua_i_subscription:
		XGLOG_INFO("SofiaSiphandler::SofiaCallBack(Event:nua_i_subscription)");
		break;
	case nua_i_state:
		XGLOG_INFO("SofiaSiphandler::SofiaCallBack(Event:nua_i_state)");
		((SofiaSiphandler *)magic)->OnStateChange(status, phrase, nua, magic, nh, hmagic, sip, tags);
		break;
	case nua_i_terminated:
		XGLOG_INFO("SofiaSiphandler::SofiaCallBack(Event:nua_i_terminated)");
		break;
		//................
		// SIP Request Event Handler Function
	case nua_i_ack:
		XGLOG_INFO("SofiaSiphandler::SofiaCallBack(Event:nua_i_ack)");
		((SofiaSiphandler *)magic)->OnAckRequest(status, phrase, nua, magic, nh, hmagic, sip, tags);
		break;
	case nua_i_bye:
		XGLOG_INFO("SofiaSiphandler::SofiaCallBack(Event:nua_i_bye)");
		((SofiaSiphandler *)magic)->OnByeRequest(status, phrase, nua, magic, nh, hmagic, sip, tags);
		break;
	case nua_i_cancel:
		XGLOG_INFO("SofiaSiphandler::SofiaCallBack(Event:nua_i_cancel)");
		((SofiaSiphandler *)magic)->OnInviteCancel(status, phrase, nua, magic, nh, hmagic, sip, tags);
		break;
	case nua_i_chat:
		XGLOG_INFO("SofiaSiphandler::SofiaCallBack(Event:nua_i_chat)");
		break;
	case nua_i_info:
		XGLOG_INFO("SofiaSiphandler::SofiaCallBack(Event:nua_i_info)");
		((SofiaSiphandler *)magic)->OnInfoRequest(status, phrase, nua, magic, nh, hmagic, sip, tags);
		break;
	case nua_i_invite:
		XGLOG_INFO("ippbx-multithreaded : case nua_i_invite : ");
		((SofiaSiphandler *)magic)->OnInviteRequest(status, phrase, nua, magic, nh, hmagic, sip, tags);
		break;
	case nua_i_subscribe:
		XGLOG_INFO("SofiaSiphandler::SofiaCallBack(Event:nua_i_subscribe)");
		break;
	case nua_i_message:
		XGLOG_INFO("SofiaSiphandler::SofiaCallBack(Event:nua_i_message)");
		break;
	case nua_i_method:
		XGLOG_INFO("SofiaSiphandler::SofiaCallBack(Event:nua_i_method)");
		break;
	case nua_i_notify:
		XGLOG_INFO("SofiaSiphandler::SofiaCallBack(Event:nua_i_notify)");
		((SofiaSiphandler *)magic)->OnNotifyRequest(status, phrase, nua, magic, nh, hmagic, sip, tags);
		break;
	case nua_i_options:
		XGLOG_INFO("SofiaSiphandler::SofiaCallBack(Event:nua_i_options)");
		((SofiaSiphandler *)magic)->OnOptionsRequest(status, phrase, nua, magic, nh, hmagic, sip, tags);
		break;
	case nua_i_prack:
		XGLOG_INFO("SofiaSiphandler::SofiaCallBack(Event:nua_i_prack)");
		break;
	case nua_i_publish:
		XGLOG_INFO("SofiaSiphandler::SofiaCallBack(Event:nua_i_publish)");
		break;
	case nua_i_refer:
		XGLOG_INFO("SofiaSiphandler::SofiaCallBack(Event:nua_i_refer)");
		((SofiaSiphandler *)magic)->OnReferRequest(status, phrase, nua, magic, nh, hmagic, sip, tags);
		break;
	case nua_i_register:
		XGLOG_INFO("SofiaSiphandler::SofiaCallBack(Event:nua_i_register)");
		((SofiaSiphandler *)magic)->OnRegistrationRequest(status, phrase, nua, magic, nh, hmagic, sip, tags);
		break;
	case nua_i_update:
		XGLOG_INFO("SofiaSiphandler::SofiaCallBack(Event:nua_i_update)");
		break;
	case nua_r_get_params:
		XGLOG_INFO("SofiaSiphandler::SofiaCallBack(Event:nua_r_get_params)");
		break;
	case nua_r_notifier:
		XGLOG_INFO("SofiaSiphandler::SofiaCallBack(Event:nua_r_notifier)");
		break;
	case nua_r_shutdown:
		XGLOG_INFO("SofiaSiphandler::SofiaCallBack(Event:nua_r_shutdown)");
		break;
	case nua_r_terminate:
		XGLOG_INFO("SofiaSiphandler::SofiaCallBack(Event:nua_r_terminate)");
		break;
		//-----------------------
		// SIP Response Event Handler Function
	case nua_r_bye:
		XGLOG_INFO("SofiaSiphandler::SofiaCallBack(Event:nua_r_bye)");
		break;
	case nua_r_cancel:
		XGLOG_INFO("SofiaSiphandler::SofiaCallBack(Event:nua_r_cancel)");
		break;
	case nua_r_info:
		XGLOG_INFO("SofiaSiphandler::SofiaCallBack(Event:nua_r_info)");
		break;
	case nua_r_invite:
		XGLOG_INFO("SofiaSiphandler::SofiaCallBack(Event:nua_r_invite)");
		((SofiaSiphandler *)magic)->OnRequestResponse(status, phrase, nua, magic, nh, hmagic, sip, tags);
		break;
	case nua_r_message:
		XGLOG_INFO("SofiaSiphandler::SofiaCallBack(Event:nua_r_message)");
		break;
	case nua_r_notify:
		XGLOG_INFO("SofiaSiphandler::SofiaCallBack(Event:nua_r_notify)");
		break;
	case nua_r_options:
		XGLOG_INFO("SofiaSiphandler::SofiaCallBack(Event:nua_r_options)");
		((SofiaSiphandler *)magic)->OnOptionsResponse(status, phrase, nua, magic, nh, hmagic, sip, tags);
		break;
	case nua_r_prack:
		XGLOG_INFO("SofiaSiphandler::SofiaCallBack(Event:nua_r_prack)");
		break;
	case nua_r_publish:
		XGLOG_INFO("SofiaSiphandler::SofiaCallBack(Event:nua_r_publish)");
		break;
	case nua_r_refer:
		XGLOG_INFO("SofiaSiphandler::SofiaCallBack(Event:nua_r_refer)");
		break;
	case nua_r_register:
		XGLOG_INFO("SofiaSiphandler::SofiaCallBack(Event:nua_r_register)");
		((SofiaSiphandler *)magic)->OnRegisterResponse(status, phrase, nua, magic, nh, hmagic, sip, tags);
		break;
	case nua_r_subscribe:
		XGLOG_INFO("SofiaSiphandler::SofiaCallBack(Event:nua_r_subscribe)");
		break;
	case nua_r_unpublish:
		XGLOG_INFO("SofiaSiphandler::SofiaCallBack(Event:nua_r_unpublish)");
		break;
	case nua_r_unregister:
		XGLOG_INFO("SofiaSiphandler::SofiaCallBack(Event:nua_r_unregister)");
		break;
	case nua_r_unsubscribe:
		XGLOG_INFO("SofiaSiphandler::SofiaCallBack(Event:nua_r_unsubscribe)");
		break;
	case nua_r_update:
		XGLOG_INFO("SofiaSiphandler::SofiaCallBack(Event:nua_r_update)");
		break;
	default: // An unknown event can always be handled in the event callback
		XGLOG_INFO("SofiaSiphandler::SofiaCallBack UNKOWN EVENT");
		if (status < 200 && nua_event_is_incoming_request(event))
		{
			nua_respond(nh, SIP_501_NOT_IMPLEMENTED, NUTAG_WITH_THIS(nua), TAG_END());
			if (hmagic == nullptr)
				nua_handle_destroy(nh);
		}

		break;
	}
}

void SofiaSiphandler::OnStateChange(int status, char const *phrase, nua_t *nua,
									nua_magic_t *magic, nua_handle_t *nh, nua_hmagic_t *hmagic, sip_t const *sip,
									tagi_t tags[])
{

	if (!hmagic)
		return;
	xGateSofiaobj *sofiaobjsiphandle;

	sofiaobjsiphandle = (xGateSofiaobj *)hmagic;

	tl_gets(tags, NUTAG_CALLSTATE_REF(sofiaobjsiphandle->Sipcallstate), TAG_END());

	XGLOG_INFO("SofiaSiphandler::OnStateChange (%d)", sofiaobjsiphandle->Sipcallstate);

	switch (sofiaobjsiphandle->Sipcallstate)
	{
	case nua_callstate_init:
		break;
	case nua_callstate_authenticating:
		break;
	case nua_callstate_calling: // Invite Sent
		break;
	case nua_callstate_proceeding:
		RespondToPeer(status, sofiaobjsiphandle->SofiaLegB);
		break;
	case nua_callstate_ready:
	{
		RespondToPeer(status, sofiaobjsiphandle->SofiaLegB);
		sofiaobjsiphandle->isAckReceived = true; // TODO:setting isAckReceived is true after sending ack
	}
	break;
	case nua_callstate_completing:
	{
		// RespondToPeer(EN_200_REL,sofiaobjsiphandle->SofiaLegB);
	}
	break;
	case nua_callstate_received:
	{
		cout << "#$$$$$$$$$$$$$$INVITE :nua_callstate_received $$$$$$$$$$$$$$" << endl;
		if (sofiaobjsiphandle->callstate >= EN_CS_CONNECTED)
		{
			cout << "#$$$$$$$$$$$$Re-Invite :nua_callstate_received $$$$$$$$$$$$" << endl;
			sofiaobjsiphandle->mediastate = EN_MED_SET;
			ForwardReInvite(sofiaobjsiphandle);
		}
		if (sofiaobjsiphandle->callstate < EN_CS_CONNECTED && !sofiaobjsiphandle->cp.isInboundPstnCall && !sofiaobjsiphandle->cp.isMsTeamsCall)
		{
			sofiaobjsiphandle->mediastate = EN_MED_SET;
			ProcessWssInviteRequest(nh, (sip_t *)sofiaobjsiphandle->siptag, tags, sofiaobjsiphandle->callid, 0);
		}
	}
	break;

	case nua_callstate_completed:
	{
		if (sofiaobjsiphandle->callstate == EN_CS_SUSPENDED)
		{
			ForwardReInvite(sofiaobjsiphandle);
		}
	}
	break;
	case nua_callstate_terminating:
	{
		DeleteConnectedList(sofiaobjsiphandle->uid, sofiaobjsiphandle, XGATEOBJECT::EN_OT_ORIGINATION);
		DeleteSofiaObject(sofiaobjsiphandle);
		sofiaobjsiphandle = nullptr;
	}
	break;
	case nua_callstate_terminated:
	{
#if 1//Siva :: we have to carefully handle when we use 302 redirect
			DeleteConnectedList(sofiaobjsiphandle->uid,sofiaobjsiphandle,XGATEOBJECT::EN_OT_ORIGINATION);
			DeleteSofiaObject(sofiaobjsiphandle);
			sofiaobjsiphandle = nullptr;
#endif
	}
		break;
	default:
		break;
	}
}

bool SofiaSiphandler::OnRegistrationRequest(int status, char const *phrase, nua_t *nua,
											nua_magic_t *magic, nua_handle_t *nh, nua_hmagic_t *hmagic, sip_t const *sip,
											tagi_t tags[])
{
	XGLOG_INFO("SofiaSiphandler", " OnRegistrationRequest");

	// Validating From,To,Contact, CallId Header
	if (ValidateSipMessage(sip, SIPVALIDATE::FROM) == false ||
		ValidateSipMessage(sip, SIPVALIDATE::TO) == false ||
		ValidateSipMessage(sip, SIPVALIDATE::CONTACT) == false ||
		ValidateSipMessage(sip, SIPVALIDATE::CALLID) == false)
	{

		SofiaRespondWithSavedEvent(nh, 400, "Bad Request");

		XGLOG_INFO("inside SofiaSiphandler::OnRegistrationRequest() Header validation failed");
		SofiaHandleDestroy(nh);
		return false;
	}

	/* Fraud call verification*/
	std::string host = sip->sip_from->a_url->url_host;
	if (!hmagic)
	{
		if (VerifyForFraudCall(host) == false)
		{

			SofiaRespondWithSavedEvent(nh, 403, "Forbidden"); 
			XGLOG_INFO("inside SofiaSiphandler::OnRegistrationRequest() ForFraudCall identified");
			SofiaHandleDestroy(nh);
			return false;
		}
	}

	if (sip && sip->sip_authorization)
	{
		handleClientRegRequestWithAuth(nh, sip, magic, hmagic);
	}
	else
	{
		handleClientRegRequestWOAuth(nh, sip);
	}
	return true;
}

/**/

void SofiaSiphandler::OnInviteRequest(int status, char const *phrase, nua_t *nua,
									  nua_magic_t *magic, nua_handle_t *nh, nua_hmagic_t *hmagic, sip_t const *sip,
									  tagi_t tags[])
{
	XGLOG_FUNCTION_EN_EX("");

	if (ValidateSipMessage(sip, SIPVALIDATE::FROM) == false ||
		ValidateSipMessage(sip, SIPVALIDATE::CONTACT) == false ||
		ValidateSipMessage(sip, SIPVALIDATE::CALLID) == false)
	{
		nua_respond(nh, 400, "Bad Request", TAG_END());
		XGLOG_INFO("inside SofiaSiphandler::OnInviteRequest() Header validation failed");
		return;
	}
	/* Fraud call verification*/

	std::string host = sip->sip_from->a_url->url_host;
	if (!hmagic)
	{
		if (VerifyForFraudCall(host) == false)
		{
			nua_respond(nh, 403, "Forbidden-Source Endpoint Lookup Failed", TAG_END());
			return;
		}
	}

	XGLOG_INFO("SofiaSiphandler::OnInviteRequest() %s:%s", sip->sip_from->a_url->url_user, sip->sip_from->a_url->url_host);
	// Creating call id

	std::string callId = CreateUniqueCallid(sip->sip_call_id->i_id,
											sip->sip_from->a_tag);
	callId += '\0';

	XGLOG_INFO("SofiaSiphandler::OnInviteRequest() callid  %s", callId.c_str());

	xGateSofiaobj *sofiaobj = nullptr;
	XGATESIPEVENT sipevent;
	if (hmagic)
	{

		sofiaobj = (xGateSofiaobj *)hmagic;
		if (sip && sip->sip_via)
			sofiaobj->siptransport = sip->sip_via->v_protocol;

		if (sip && sip->sip_payload && sip->sip_payload->pl_data)
		{
			std::cout << "$$$$$$$$$$$$$$$$ REINVITE setting SDP string $$$$$$$$$$$$$$$$$ " << sofiaobj->callid.c_str() << std::endl;
			// sofiaobj->cp.sdpStr = sip->sip_payload->pl_data;
			sofiaobj->m_sOffSdp = sip->sip_payload->pl_data;
		}
		XGLOG_INFO("SofiaSiphandler::OnInviteRequest() Re-Invite Request  %s", sofiaobj->uid.c_str());
		switch (sofiaobj->callstate)
		{
		case EN_CS_PROCEEDING:
		case EN_CS_ALERTING:
			nua_respond(nh, 400, "Bad Request", TAG_END());
			XGLOG_ERROR("inside SofiaSiphandler::OnInviteRequest()call in progress %s", sofiaobj->uid.c_str());
			return;
		case EN_CS_CONNECTED:

			sipevent = EN_HOLDRESUME_REQ;
			XGLOG_INFO("SofiaSiphandler::OnInviteRequest() Re-Invite Suspended  %s", sofiaobj->uid.c_str());

			XGLOG_INFO("SofiaSiphandler::OnInviteRequest callstate = CS_Connected");
			sofiaobj->callstate = EN_CS_SUSPENDED;
			if (sofiaobj->SofiaLegB)
			{
				XGLOG_INFO("SofiaSiphandler::OnInviteRequest changing SofiaLegB callstate to CS_Suspended");
				sofiaobj->SofiaLegB->callstate = EN_CS_SUSPENDED;
			}

			break;
		case EN_CS_SUSPENDED:
			// Set The call state to CS_Conecting
			sofiaobj->callstate = EN_CS_CONNECTED;
			sofiaobj->mediastate = EN_MED_RESUMED;
			sipevent = EN_HOLDRESUME_REQ;
			XGLOG_INFO("SofiaSiphandler::OnInviteRequest() Re-Invite Resumed %s", sofiaobj->uid.c_str());

			XGLOG_INFO("SofiaSiphandler::OnInviteRequest callstate = CS_Suspended");
			if (sofiaobj->SofiaLegB)
			{
				XGLOG_INFO("SofiaSiphandler::OnInviteRequest changing SofiaLegB callstate to CS_Connected");
				sofiaobj->SofiaLegB->callstate = EN_CS_CONNECTED;
			}
			break;

		default:
			sipevent = EN_REINVITE_REQ;
			XGLOG_INFO("SofiaSiphandler::OnInviteRequest() Re-Invite on timeout %s", sofiaobj->uid.c_str());
			break;
		}
		return;
	}
	handleInviteRequest(callId, nh, sip, magic, hmagic,tags);
	return;
}

void SofiaSiphandler::OnInviteCancel(int status, char const *phrase, nua_t *nua,
									 nua_magic_t *magic, nua_handle_t *nh, nua_hmagic_t *hmagic, sip_t const *sip,
									 tagi_t tags[])
{
	XGLOG_FUNCTION_EN_EX("");

	if (!hmagic)
	{
		XGLOG_ERROR("hmagic is null");
		return;
	}

	XGLOG_INFO("SofiaSiphandler::OnInviteCancel(%s)", ((xGateSofiaobj *)hmagic)->callid.c_str());

	xGateSofiaobj *sobj = nullptr;
	xGateCallPair *callpair = nullptr;

	callpair = GetConnectedList(((xGateSofiaobj *)hmagic)->callid.c_str());
	if (callpair)
	{
		sobj = callpair->Orgcall;
		if (sobj)
		{
			SofiaRespond(nh, 200, "OK");
			if (((xGateSofiaobj *)hmagic)->SofiaLegB && ((xGateSofiaobj *)hmagic)->isPeerInviteSent)
			{
				if (((xGateSofiaobj *)hmagic)->SofiaLegB->sofianh)
				{
					SofiaSendCancel((nua_handle_t *)((xGateSofiaobj *)hmagic)->SofiaLegB->sofianh);
					//((xGateSofiaobj*)hmagic)->SofiaLegB->isCancelSent = true;
					if (((xGateSofiaobj *)hmagic)->isInviteTimerStarted)
					{
						cancel_invite_timer(sobj);
					}
				}
			}
			else
			{
				XGLOG_INFO("SofiaSiphandler::OnInviteCancel Skipping Cancel .Dialout not yet done for Peer Leg %s", ((xGateSofiaobj *)hmagic)->callid.c_str());
			}
		}
	}
}

/* OnInfoRequest handels the sip info request from sofia stack event
 *
 * */
void SofiaSiphandler::OnInfoRequest(int status, char const *phrase, nua_t *nua,
									nua_magic_t *magic, nua_handle_t *nh, nua_hmagic_t *hmagic, sip_t const *sip,
									tagi_t tags[])
{

	XGLOG_INFO("SofiaSiphandler::OnInfoRequest : Enter function");

	if (sip && sip->sip_payload && sip->sip_payload->pl_data)
	{
		xGateSofiaobj *sofiaobj = nullptr;
		if (hmagic)
		{
			sofiaobj = (xGateSofiaobj *)hmagic;
			sofiaobj->siptransport = ((sip_t *)sip)->sip_via->v_protocol;
			sip_payload_t *payload = sip_payload_dup(&m_homeMemory, sip->sip_payload);
			sofiaobj->m_sInfoContentType = sip->sip_content_type->c_type;
			if (sofiaobj->SofiaLegB->isAckReceived == true)
			{
				ForwardSipInfo(sofiaobj, payload);
			}
			else
			{
				sofiaobj->InfoPayload = (void *)payload; // TODO::Siva This is temp fix ,we need to disbale auto ack from stack and implement application layer
				sofiaobj->SofiaLegB->isSipInfoPending = true;
			}
		}
	}
	else
	{
		XGLOG_INFO("SofiaSiphandler::OnInfoRequest : SIP-Info message does NOT contain message-body");
	}
}

void SofiaSiphandler::OnByeRequest(int status, char const *phrase, nua_t *nua,
								   nua_magic_t *magic, nua_handle_t *nh, nua_hmagic_t *hmagic, sip_t const *sip,
								   tagi_t tags[])
{

	if (!hmagic)
		return;

	XGLOG_INFO("SofiaSiphandler::OnByeRequest(%s)", ((xGateSofiaobj *)hmagic)->callid.c_str());

	xGateSofiaobj *sobj = nullptr;
	xGateCallPair *callpair = nullptr;

	callpair = GetConnectedList(((xGateSofiaobj *)hmagic)->callid.c_str());
	if (callpair)
	{
		sobj = callpair->Orgcall;
		if (sobj)
		{
			SofiaRespond(nh, 200, "OK");
			if (((xGateSofiaobj *)hmagic)->SofiaLegB)
			{
				nua_bye((nua_handle_t *)((xGateSofiaobj *)hmagic)->SofiaLegB->sofianh, TAG_END());
			}
		}
	}
}

void SofiaSiphandler::OnNotifyRequest(int status, char const *phrase, nua_t *nua,
									  nua_magic_t *magic, nua_handle_t *nh, nua_hmagic_t *hmagic, sip_t const *sip,
									  tagi_t tags[])
{
	XGLOG_FUNCTION_EN_EX("");
	// TODO:For debugging bye issue
	return;

	if (!hmagic)
		return;

	XGLOG_INFO("SofiaSiphandler::OnNotifyRequest(%s)", ((xGateSofiaobj *)hmagic)->callid.c_str());
	// SofiaRespond(nh,200,"OK");

	nua_handle_t *handle = nullptr;
	xGateSofiaobj *sofiaobj = nullptr;

	handle = (nua_handle_t *)((xGateSofiaobj *)hmagic)->SofiaLegB->sofianh;

	nua_notify(handle, TAG_END());
}

void SofiaSiphandler::OnOptionsRequest(int status, char const *phrase, nua_t *nua,
									   nua_magic_t *magic, nua_handle_t *nh, nua_hmagic_t *hmagic, sip_t const *sip,
									   tagi_t tags[])
{
	XGLOG_FUNCTION_EN_EX("");
	if (ValidateSipMessage(sip, SIPVALIDATE::FROM) == false ||
		ValidateSipMessage(sip, SIPVALIDATE::CALLID) == false ||
		ValidateSipMessage(sip, SIPVALIDATE::VIA) == false )
	{

		SofiaRespondWithSavedEvent(nh, 400, "Bad Request");

		XGLOG_INFO("inside SofiaSiphandler::OnOptionsRequest() Header validation failed");
		return;
	}
	/* Fraud call verification*/

	std::string host = sip->sip_from->a_url->url_host;
	std::string rhost {};
	if(sip->sip_via && sip->sip_via->v_received)
	   rhost = sip->sip_via->v_received;
	if (!hmagic)
	{
		if ((VerifyForFraudCall(host) == false) && VerifyForFraudCall(rhost) == false)
		{

			SofiaRespondWithSavedEvent(nh, 403, "Forbidden");
			XGLOG_INFO("inside SofiaSiphandler::OnOptionsRequest() FraudCall identidied");
			return;
		}
	}

	//if OPTION not comming from PBX sending 200 OK 
	if(!isPBXCall(sip))
	{
		SofiaRespondWithSavedEvent(nh, 200, "OK");
		return;
	}

	XGLOG_INFO("SofiaSiphandler::OnOptionsRequest() %s:%s", sip->sip_from->a_url->url_user, sip->sip_from->a_url->url_host);
	// Creating call id
	std::string callidstring = CreateUniqueCallid(sip->sip_from->a_url->url_user,
												  sip->sip_from->a_url->url_host,
												  sip->sip_from->a_tag,
												  sip->sip_call_id->i_id);
	callidstring += '\0';
	xGateSofiaobj *sofiaobj = nullptr;

	sofiaobj = CreateSofiaOptionsobj(callidstring, nh, magic, (sip_t *)sip);
	if (!sofiaobj)
	{
		XGLOG_INFO("SofiaSiphandler::OnOptionsRequest called sofiaobj is empty");
		return;
	}

	ProcessOptionsRequest(sofiaobj, (sip_t *)sip, tags, callidstring);
}

void SofiaSiphandler::OnAckRequest(int status, char const *phrase, nua_t *nua,
								   nua_magic_t *magic, nua_handle_t *nh, nua_hmagic_t *hmagic, sip_t const *sip,
								   tagi_t tags[])
{
	if (!hmagic)
		return;

	XGLOG_INFO("SofiaSiphandler::OnAckRequest(%s)", ((xGateSofiaobj *)hmagic)->callid.c_str());

	xGateSofiaobj *sofiaobj = nullptr;

	sofiaobj = (xGateSofiaobj *)hmagic;

	if (sofiaobj)
	{
		sofiaobj->isAckReceived = true;
	}

	if (sofiaobj->isSipInfoPending == true)
	{
		ForwardSipInfo(sofiaobj->SofiaLegB, (sip_payload_t *)sofiaobj->SofiaLegB->InfoPayload);
		XGLOG_INFO("SofiaSiphandler::OnAckRequest(%s) INFO forwarded to client", ((xGateSofiaobj *)hmagic)->callid.c_str());
		sofiaobj->isSipInfoPending = false;
	}
}

void SofiaSiphandler::OnReferRequest(int status, char const *phrase, nua_t *nua, nua_magic_t *magic, nua_handle_t *nh,
									 nua_hmagic_t *hmagic, sip_t const *sip, tagi_t tags[])
{
	XGLOG_FUNCTION_EN_EX("");

	if (!hmagic)
		return;

	XGLOG_INFO("SofiaSiphandler::OnReferRequest(%s)", ((xGateSofiaobj *)hmagic)->callid.c_str());

	xGateSofiaobj *sobj = nullptr;
	xGateCallPair *callpair = nullptr;

	callpair = GetConnectedList(((xGateSofiaobj *)hmagic)->callid.c_str());
	if (callpair)
	{
		sobj = callpair->Orgcall;
		if (sobj)
		{
			SofiaRespond(nh, 200, "OK");

			xGateSofiaobj *sofiaobj = nullptr;
			sofiaobj = (xGateSofiaobj *)hmagic;
			char const *params = (char const *)sip->sip_refer_to->r_url->url_headers;
			if (params == nullptr)
			{
				XGLOG_INFO("SofiaSiphandler::OnReferRequest Refer To Header is not found");
				ForwardReferRequest(sofiaobj, status, phrase, nua, magic, nh, hmagic, sip, tags);
			}
			else
			{
				XGLOG_INFO("SofiaSiphandler::OnReferRequest Refer To Header is found");
				//OnReferRequestAttendent(sofiaobj, status, phrase, nua, magic, nh, hmagic, sip, tags);//Currently not used because PBX has org call id logic 
				ForwardReferRequest(sofiaobj, status, phrase, nua, magic, nh, hmagic, sip, tags);
			}
		}
	}
}

void SofiaSiphandler::OnRegisterResponse(int status, char const *phrase, nua_t *nua,
										 nua_magic_t *magic, nua_handle_t *nh, nua_hmagic_t *hmagic, sip_t const *sip,
										 tagi_t tags[])
{

	XGLOG_INFO("SofiaSiphandler::OnRegisterResponse(%d)", status);

	switch (status)
	{
	case 401:
	{
		auth_MD5(nh, sip);
		break;
	}
	case 200:
	{
		XGLOG_INFO("SofiaSiphandler::OnRegisterResponse 200 OK received");
		break;
	}
	default:
	{
		break;
	}
	}
}

void SofiaSiphandler::OnRequestResponse(int status, char const *phrase, nua_t *nua,
										nua_magic_t *magic, nua_handle_t *nh, nua_hmagic_t *hmagic, sip_t const *sip,
										tagi_t tags[])
{

	XGLOG_FUNCTION_EN_EX("");

	if (!hmagic)
	{
		XGLOG_ERROR("hmagic is null");
		return;
	}
	if (!sip)
	{
		XGLOG_ERROR("sip is null");
		return;
	}

	XGLOG_INFO("SofiaSiphandler::OnRequestResponse(%d)", status);
	((xGateSofiaobj *)hmagic)->siptag = (void *)sip;
	if (((xGateSofiaobj *)hmagic)->SofiaLegB)
		((xGateSofiaobj *)hmagic)->SofiaLegB->sipeventtype = (XGATESIPEVENT)status;

	if (((xGateSofiaobj *)hmagic)->SofiaLegB && ((xGateSofiaobj *)hmagic)->SofiaLegB->isInviteTimerStarted && status != 487)
	{
		cancel_invite_timer(((xGateSofiaobj *)hmagic)->SofiaLegB);
	}

	switch (status)
	{
	case 183:
	{
		XGLOG_INFO("$$$$$$$$$$$$$$$$$$$$$ RECVD 183 Processing $$$$$$$$$$$$$$$$", ((xGateSofiaobj *)hmagic)->uid.c_str());
		((xGateSofiaobj *)hmagic)->SofiaLegB->m_sAnsSdp = "";
		if (sip && sip->sip_payload && sip->sip_payload->pl_data)
			((xGateSofiaobj *)hmagic)->SofiaLegB->m_sAnsSdp = (char *)sip->sip_payload->pl_data;
		if (((xGateSofiaobj *)hmagic)->callstate < EN_CS_PROCEEDING)
		{
			((xGateSofiaobj *)hmagic)->callstate = EN_CS_PROCEEDING;
		}
	}
	break;
	case 180:
	{
		XGLOG_INFO("$$$$$$$$$$$$$$$$$$$$$ RECVD 180 Ringing $$$$$$$$$$$$$$$$", ((xGateSofiaobj *)hmagic)->uid.c_str());
		((xGateSofiaobj *)hmagic)->SofiaLegB->m_sAnsSdp = "";
		if (sip && sip->sip_payload && sip->sip_payload->pl_data)
			((xGateSofiaobj *)hmagic)->SofiaLegB->m_sAnsSdp = (char *)sip->sip_payload->pl_data;
		if (((xGateSofiaobj *)hmagic)->callstate < EN_CS_ALERTING)
		{
			((xGateSofiaobj *)hmagic)->callstate = EN_CS_ALERTING;
		}
	}
	break;
		;
	case 200:
	{
		XGLOG_INFO("$$$$$$$$$$$$$$$$$$$$$ RECVD 200 OK $$$$$$$$$$$$$$$$", ((xGateSofiaobj *)hmagic)->uid.c_str());
		((xGateSofiaobj *)hmagic)->SofiaLegB->m_sAnsSdp = (char *)sip->sip_payload->pl_data;

		// copying totag & Responsecallid into A 's Sofia object
		((xGateSofiaobj *)hmagic)->replaceinfo.to_tag = (char const *)sip->sip_to->a_tag;
		((xGateSofiaobj *)hmagic)->replaceinfo.Responsecallid = (char const *)sip->sip_call_id->i_id;

		// copying totag & Responsecallid into B 's Sofia object
		((xGateSofiaobj *)hmagic)->SofiaLegB->replaceinfo.to_tag = (char const *)sip->sip_to->a_tag;
		((xGateSofiaobj *)hmagic)->SofiaLegB->replaceinfo.Responsecallid = (char const *)sip->sip_call_id->i_id;

		// Copying A's from tag and to invite callid to B
		((xGateSofiaobj *)hmagic)->replaceinfo.from_tag = ((xGateSofiaobj *)hmagic)->SofiaLegB->replaceinfo.from_tag;
		((xGateSofiaobj *)hmagic)->replaceinfo.Invitecallid = ((xGateSofiaobj *)hmagic)->SofiaLegB->replaceinfo.Invitecallid;

		if (sip && sip->sip_user_agent && sip->sip_user_agent->g_string)
		{
			((xGateSofiaobj *)hmagic)->cp.calledDeviceType = sip->sip_user_agent->g_string;
		}
	}

	break;
	case 302:
	{
		XGLOG_INFO("SofiaSiphandler::OnRequestResponse Received 302 Response from MGC");
		xGateSofiaobj *ptLegA = ((xGateSofiaobj *)hmagic)->SofiaLegB;
		ptLegA->cp.is302messagearrived = true;
		ptLegA->cp.forwardingmgcrouteIp = sip->sip_contact->m_url->url_host;

		RedirectOnReceiving302(ptLegA);
		break;
	}
	case 486:
		// callobj->callcauseB = CC_UserBusy;
		break;
	case 487:
		break;
	case EN_403_REL:
		// callobj->callcauseB = CC_CallRejected;
		break;
	case 480:
	case 404:
	case 503:
	{

#if 0 //Siva ::Currenlty we are not using reroute logic since its handled in interrogator 
		if (strstr(sip->sip_via->v_protocol, "SIP/2.0/UDP"))
		{
			if (!((xGateSofiaobj *)hmagic)->SofiaLegB->isCallReRouted && ((xGateSofiaobj *)hmagic)->callstate != EN_CS_SUSPENDED &&
				((xGateSofiaobj *)hmagic)->callstate != EN_CS_CONNECTED)
			{
				xGateSofiaobj *ptLegA = nullptr;
				ptLegA = ((xGateSofiaobj *)hmagic)->SofiaLegB;
				ptLegA->cp.isServerFailure = true;
				XGLOG_INFO(" SofiaSiphandler::OnRequestResponse Received %d ", status);
				ReRouteCallToOtherNode(ptLegA);
			}
			else
				RespondToPeer(503, ((xGateSofiaobj *)hmagic)->SofiaLegB);
			XGLOG_ERROR("SofiaSiphandler::OnRequestResponse 503 Received!");
		}
		else
#endif
		{
			RespondToPeer(status, ((xGateSofiaobj *)hmagic)->SofiaLegB);
			XGLOG_ERROR("SofiaSiphandler::OnRequestResponse sending Response to Peer !");
		}

		break;
	}
	default:
		// if(status > 399)
		//    callobj->callcauseB = CC_NormalUnspecified;
		break;
	}

	if (status > 399 && status != 487 && status != 503 && status != 480 && status != 404)
	{
		XGLOG_WARN("SofiaSiphandler::OnRequestResponse sending 4xx Responses to peer");
		RespondToPeer(status, ((xGateSofiaobj *)hmagic)->SofiaLegB);
	}
}

void SofiaSiphandler::OnOptionsResponse(int status, char const *phrase, nua_t *nua,
										nua_magic_t *magic, nua_handle_t *nh, nua_hmagic_t *hmagic, sip_t const *sip,
										tagi_t tags[])
{

	if (!hmagic)
	{
		XGLOG_INFO("SofiaSiphandler::OnOptionsResponse hmagic is nullptr");
		return;
	}
	xGateSofiaobj *sobj = nullptr;
	sobj = (xGateSofiaobj *)hmagic;
	if(sobj && sobj->isTrunkCall)
	{
		XGLOG_INFO("SofiaSiphandler::OnOptionsResponse for trunk call");
		handleOptionsResponse(status,sobj);
		return;
	}
	sobj->SofiaLegB->sipeventtype = (XGATESIPEVENT)status;

	if (!sobj)
		return;

	SofiaRespond((nua_handle_t *)((xGateSofiaobj *)hmagic)->SofiaLegB->sofianh, status, phrase,
				 (nua_saved_event_t const *)sobj->SofiaLegB->sofiahmsg);

	DestroySavedEvent((nua_saved_event_t *)sobj->SofiaLegB->sofiahmsg);
}

bool SofiaSiphandler::handleOptionsResponse(int status, xGateSofiaobj *sobj)
{
	if (!sobj)
	{
		XGLOG_WARN("SofiaSiphandler::handleOptionResponse failed sobj is nullptr");
		return false;
	}

	xGateCallObj *callobj = new xGateCallObj();

	callobj->m_sRouteIP = sobj->m_sRouteIP.c_str();
	callobj->m_sCallingNumber = sobj->m_sCallingNumber.c_str();
	callobj->uid = sobj->callid.c_str();
	callobj->m_iTrunkId = sobj->m_iTrunkId;
	callobj->m_RespCode = status;

	if (status == 200)
	{
		XGLOG_WARN("SofiaSiphandler::handleOptionResponse XGATESIPEVENT::EN_OPTIONS_SUCCESS_RESPONSE for callid(%s)" ,sobj->callid.c_str());
		callobj->sipeventtype = XGATESIPEVENT::EN_OPTIONS_SUCCESS_RESPONSE;
	}
	else
	{
		XGLOG_WARN("SofiaSiphandler::handleOptionResponse XGATESIPEVENT::EN_OPTIONS_FAILED_RESPONSE for callid(%s)",sobj->callid.c_str());
		callobj->sipeventtype = XGATESIPEVENT::EN_OPTIONS_FAILED_RESPONSE;
	}
	PostMessage(callobj, XGATEPOSTTYPE::EN_PROCESS);
	return true;
}

void SofiaSiphandler::RespondToPeer(int status, xGateSofiaobj *sobj)
{
	if (!sobj)
	{
		XGLOG_WARN("SofiaSiphandler::RespondToPeer Error - Invalid sobj");
		return;
	}
	XGLOG_INFO("SofiaSiphandler::RespondToPeer Event(%d - callid %s) ", (status / 100), sobj->uid.c_str());
	sobj->sipeventtype = (XGATESIPEVENT)status;

	switch (status / 100)
	{
	case 1:
		RespondWith1xx(sobj);
		break;
	case 2:
		RespondWith2xx(sobj);
		break;
	case 3:
		RespondWith3xx(sobj);
		break;
	case 4:
		RespondWith4xx(sobj);
		break;
	case 5:
		RespondWith5xx(sobj);
		break;
	case 6:
		RespondWith6xx(sobj);
		break;
	default:
		break;
	}
}

void SofiaSiphandler::SofiaSendCancel(nua_handle_t *nh)
{
	nua_cancel(nh, TAG_END());
}

/*
* response send with Reason header
*/
void SofiaSiphandler::ResponseWithReasonHeader(nua_handle_t *nh,int status, char const *phrase , const char *reson_header)
{
	XGLOG_INFO("SofiaSiphandler::ResponseWithReasonHeader send 408 response to SGW ==> PBX  with Client Connection Timeout reason header ");
	nua_respond(nh,status,phrase,TAG_IF(reson_header[0] != '\0',SIPTAG_REASON_STR(reson_header)),TAG_END());
}


/*
* Loading Sip params for Sofia handler
*/
bool SofiaSiphandler::LoadSipHandlerParams()
{

	m_pSipHndlrParams = new SipHandlerParams();
	m_pSipHndlrParams->m_sPbxUA = CONFIGSERVICE->m_lightWeightWSSProfile.m_pbxUA.c_str();
	m_pSipHndlrParams->m_iRespTimeOut = CONFIGSERVICE->m_lightWeightWSSProfile.m_server_resp_timeout;
	m_pSipHndlrParams->m_sSipURL = CONFIGSERVICE->m_lightWeightWSSProfile.m_sipURL.c_str();
	m_pSipHndlrParams->m_sSipsURL = CONFIGSERVICE->m_lightWeightWSSProfile.m_sipsURL.c_str();
	m_pSipHndlrParams->m_sWssURL = CONFIGSERVICE->m_lightWeightWSSProfile.m_wssURL.c_str();
	m_pSipHndlrParams->m_sSipPort = CONFIGSERVICE->m_lightWeightWSSProfile.m_sipPort.c_str();
	m_pSipHndlrParams->m_sSipsPort = CONFIGSERVICE->m_lightWeightWSSProfile.m_sipsPort.c_str();
	m_pSipHndlrParams->m_sWssPort = CONFIGSERVICE->m_lightWeightWSSProfile.m_wssPort.c_str();
	m_pSipHndlrParams->m_sCertPath = CONFIGSERVICE->m_lightWeightWSSProfile.m_certPath.c_str();
	m_pSipHndlrParams->m_sIPv4Add = CONFIGSERVICE->m_lightWeightWSSProfile.m_contactWssIp.c_str();
	m_pSipHndlrParams->m_sIPv6Add = CONFIGSERVICE->m_lightWeightWSSProfile.m_contactWssIpv6.c_str();
	m_pSipHndlrParams->m_sDefaultPBXIP = CONFIGSERVICE->m_lightWeightWSSProfile.m_default_pbx_ip.c_str();
	m_pSipHndlrParams->m_isSbcEnabled = CONFIGSERVICE->m_lightWeightWSSProfile.m_isSbcEnabled;
	m_pSipHndlrParams->m_iServerExpires = CONFIGSERVICE->m_lightWeightWSSProfile.registrationExpiresInSec;
	m_pSipHndlrParams->m_sPrivateIP = xGateUtil::getLocalIpAddr(AF_INET, EN_XGATE_INTERFACE_PRIVATE);

	return true;
}

bool SofiaSiphandler::CreateCallobjparam(xGateCallObj *callobj, xGateSofiaobj *sobj,
										 nua_handle_t *nh, sip_t const *sip, tagi_t tags[])
{

	XGLOG_INFO("SofiaSiphandler::CreateCallobjparam(%s)", sobj->uid.c_str());
	callobj->xGateCallObjInit(); // callobj initilized

	callobj->callid = sobj->callid;
	callobj->uid = sobj->uid;

	// Create Call Pararm
	SetcallInfoParameter(callobj->cp, sip, tags);
	callobj->cp.call_id = callobj->callid;
	callobj->cp.uid = sobj->uid;
	callobj->cp.tUnKnownHeaderMap = sobj->cp.tUnKnownHeaderMap;
	// Set Transport
	if (strcasecmp(sip->sip_contact->m_url->url_scheme, "sip") == 0)
	{
		if (ValidateSipMsgPointer(sip->sip_contact->m_url->url_params))
			nua_set_hparams(nh,
							NUTAG_M_PARAMS(sip->sip_contact->m_url->url_params),
							TAG_END());
	}

	sobj->cp = callobj->cp;
	return true;
}

xGateSofiaobj *SofiaSiphandler::CreateSofiaobjparam(std::string callid, nua_handle_t *nh,nua_magic_t *magic,
													 sip_t const *sip,CALL_TYPE callType=CALL_TYPE::EN_UNKNOWN_CALL)
{
	XGLOG_FUNCTION_EN_EX("");
	xGateSofiaobj *sofiaobj = new xGateSofiaobj(); // create new Dialog
	if (!sofiaobj)
	{
		XGLOG_ERROR("SofiaSiphandler::CreateSofiaobjparam failed to create xGateSofiaobj for '%s' !", callid.c_str());
		return nullptr;
	}
	if (strstr(sip->sip_via->v_protocol, "SIP/2.0/TLS") || strstr(sip->sip_via->v_protocol, "SIP/2.0/WSS"))
	{
		SofiaSetProxyRoute(nh, sip, callid.c_str());
	}
	if (isPBXCall(sip))
	{
		sofiaobj->isPbxCall = true;
	}

	sofiaobj->callstream = EN_INBOUND;
	sofiaobj->callid = callid;
	//  sofiaobj->callid += '\0'; // added for multithreaded-ippbx
	char uniqid[100] = "\0";
	snprintf(uniqid,100 ,"%s%s%s", sip->sip_from->a_url->url_user, sip->sip_to->a_url->url_user, sip->sip_call_id->i_id);
	sofiaobj->uid = callid;
	XGLOG_INFO("SofiaSiphandler::CreateSofiaobjparam(%s)", sofiaobj->uid.c_str());

	sofiaobj->sofianh = (void *)nh;	   // Transaction handle
	sofiaobj->siptag = (void *)sip; // Sip tag struct information
	sofiaobj->sofiamh = (void *)magic; // Application context

	// copying offer sdp
	if (sip && sip->sip_payload)
		sofiaobj->m_sOffSdp = (char *)((sip_t *)sip)->sip_payload->pl_data;
#if 0
	if (sip && sip->sip_via && sip->sip_via->v_host)
	{
		string sIpAddress("");
		sIpAddress = sip->sip_via->v_host;
		if (ip_validator(sIpAddress) == 1)
			sofiaobj->callLegIpAddrFamily = IP_ADDR_V4;
		else
			sofiaobj->callLegIpAddrFamily = IP_ADDR_V6;
	}
#endif
    if (sip && sip->sip_contact && sip->sip_contact->m_url->url_host)
	{
		string sIpAddress("");
		sIpAddress = sip->sip_contact->m_url->url_host;
		if (ip_validator(sIpAddress) == 1)
			sofiaobj->callLegIpAddrFamily = IP_ADDR_V4;
		else
			sofiaobj->callLegIpAddrFamily = IP_ADDR_V6;
	}

	if(sip && sip->sip_from && sip->sip_from->a_url && sip->sip_from->a_url->url_user)
		sofiaobj->m_sCallingNumber = sip->sip_from->a_url->url_user;

	if(sip && sip->sip_to && sip->sip_to->a_url && sip->sip_to->a_url->url_host)
		sofiaobj->m_sCalledNumber = sip->sip_to->a_url->url_user;

	if(sip && sip->sip_from && sip->sip_from->a_url && sip->sip_from->a_url->url_host)
         sofiaobj->m_sDomainName = sip->sip_from->a_url->url_host;

	if(callType == CALL_TYPE::EN_INBOUND_PSTN_CALL)
	  sofiaobj->cp.isInboundPstnCall = true;

	if(callType == CALL_TYPE::EN_MS_TEAMS_CALL)
	{
		auto[Available ,Number,Ext] = extractCliandExt(sofiaobj->m_sCallingNumber);
		if(Available)
		{
			sofiaobj->m_sCallingNumber = Number.c_str();
			sofiaobj->m_sCallingExt = Ext.c_str();
		}
		sofiaobj->cp.isMsTeamsCall = true;
	}

    //setting transport type
	if (sip->sip_via)
	{
		 if (strcmp(sip->sip_via->v_protocol, "SIP/2.0/TCP") == 0)
			sofiaobj->m_sTransportType = "tcp";
		 else if (strcmp(sip->sip_via->v_protocol, "SIP/2.0/UDP") == 0)
			sofiaobj->m_sTransportType = "udp";
		 else if (strcmp(sip->sip_via->v_protocol, "SIP/2.0/TLS") == 0)
			sofiaobj->m_sTransportType = "tls";
		 else if (strcmp(sip->sip_via->v_protocol, "SIP/2.0/WSS") == 0)
			sofiaobj->m_sTransportType = "wss";
		 else
			sofiaobj->m_sTransportType = "udp";
	}

	// added for Attend transfer -starts
	sofiaobj->replaceinfo.from_tag = (char const *)sip->sip_from->a_tag;
	sofiaobj->replaceinfo.Invitecallid = (char const *)sip->sip_call_id->i_id;
	// added for Attend Transfer - Ends
	//Debug Log
	XGLOG_TRACE("Invite_from(%s)_to(%s) domain(%s) callid(%s)",sip->sip_from->a_url->url_user,\
															  sip->sip_to->a_url->url_user,\
                                                              sip->sip_from->a_url->url_host,\
															  callid.c_str());
	if (ValidateSipMsgPointer(sip->sip_contact->m_url->url_params))
	{
		sofiaobj->siptransport = sip->sip_contact->m_url->url_params;
		sofiaobj->cp.csTransport = sip->sip_contact->m_url->url_params;
		sofiaobj->cp.cnCalled = sip->sip_contact->m_url->url_user; // added for multithreaded-ippbx
	}

	if (sip->sip_from && sip->sip_from->a_url && sip->sip_from->a_url->url_user)
		sofiaobj->cp.cnCalling = sip->sip_from->a_url->url_user;

	/*AOR Changes start*/
	sofiaobj->cp.m_AddrRecords.m_strProxyusername = sip->sip_from->a_url->url_user;
	if (sip->sip_contact->m_url->url_host)
	{
		sofiaobj->cp.m_AddrRecords.m_strContactaddress = sip->sip_contact->m_url->url_host;
		sofiaobj->cp.m_AddrRecords.m_strContactaddress += ":";
		if (sip->sip_via->v_rport)
		{
			sofiaobj->cp.m_AddrRecords.m_strContactaddress += sip->sip_via->v_rport;
		}
	}

	if (sofiaobj->callLegIpAddrFamily == IP_ADDR_V6)
		sofiaobj->cp.m_AddrRecords.m_strIpaddress = "[" + m_pSipHndlrParams->m_sIPv6Add + "]";
	else
		sofiaobj->cp.m_AddrRecords.m_strIpaddress = m_pSipHndlrParams->m_sIPv4Add.c_str();

	if (sip && sip->sip_user_agent && sip->sip_user_agent->g_string)
		sofiaobj->cp.m_AddrRecords.m_strDeviceType = sip->sip_user_agent->g_string;
	/*AOR Changes end*/

	//This is releted to CCCAS call
	for (sip_unknown_t *h = sip->sip_unknown; h != nullptr; h = h->un_next)
	{
		if (strcmp(h->un_name, "CCAAS-SESSION-ID") == 0)
		{
			sofiaobj->m_sKey = h->un_value;
		}
	}
	if (ValidateSipMsgPointer(sip->sip_contact->m_url->url_params))
	{
		sofiaobj->siptransport = sip->sip_contact->m_url->url_params;
		sofiaobj->cp.csTransport = sip->sip_contact->m_url->url_params;
	}

	set_resp_contact(*sofiaobj);
	if (CheckDomainForCallAuth(sip->sip_from->a_url->url_host) == false)
	{
		sofiaobj->callauthstate = 1;
	}
	else
	{
		sofiaobj->callauthstate = 0;
	}

	sofiaobj->SofiaLegB = nullptr;
	sofiaobj->SofiaLegB = new xGateSofiaobj();
	sofiaobj->SofiaLegB->xGateCopyPeerInfo(*sofiaobj);

	InsertConnectedList(sofiaobj->uid, sofiaobj, XGATEOBJECT::EN_OT_ORIGINATION);

	SofiaBindhandle(nh, (nua_hmagic_t *)sofiaobj);
	return sofiaobj;
}

bool SofiaSiphandler::CreateSofiaRegobj(xGateSofiaSipRegobj &regparam, sip_t const *sip)
{

	if (sip && sip->sip_call_id && sip->sip_call_id->i_id)
		regparam.callid = sip->sip_call_id->i_id;

	regparam.ipaddress = m_pSipHndlrParams->m_sIPv4Add.c_str();

	if (sip && sip->sip_contact && sip->sip_contact->m_url)
	{
		if (sip->sip_contact->m_url->url_user)
			regparam.proxyusername = sip->sip_contact->m_url->url_user;
		if (sip->sip_contact->m_url->url_host)
		{
			if (sip->sip_via && sip->sip_via->v_received)
			{
				if (strcmp(sip->sip_via->v_received, sip->sip_contact->m_url->url_host) == 0)
				{
					regparam.contactaddress = sip->sip_contact->m_url->url_host;
				}
				else
					regparam.contactaddress = GetValidIP(sip->sip_via->v_received);
			}
			else
			{
				if (is_public_ip(sip->sip_contact->m_url->url_host) == true)
					regparam.contactaddress = sip->sip_contact->m_url->url_host;
				else
					regparam.contactaddress = sip->sip_via->v_host;
			}
		}
	}
	if (sip->sip_via && sip->sip_via->v_rport)
	{
		regparam.contactaddress += ":";
		regparam.contactaddress += sip->sip_via->v_rport;
	}
	else if( sip->sip_contact && sip->sip_contact->m_url && sip->sip_contact->m_url->url_port)
	{
		regparam.contactaddress += ":";
		regparam.contactaddress +=  sip->sip_contact->m_url->url_port;
	}

	regparam.domainname = sip->sip_from->a_url->url_host;

	if (sip && sip->sip_to && sip->sip_to->a_url && sip->sip_to->a_url->url_user)
	{
		regparam.username = sip->sip_to->a_url->url_user;
		regparam.mobileno = sip->sip_to->a_url->url_user;
	}
	regparam.devicetype = "";
	regparam.apptype = "";

	//setting Device type
	if (sip->sip_user_agent && sip->sip_user_agent->g_string && strstr(sip->sip_user_agent->g_string, "VIDEO"))
	{
		regparam.apptype = "VIDEO";
		if (sip->sip_user_agent && sip->sip_user_agent->g_string)
		{
			if (strstr(sip->sip_user_agent->g_string, "IOS"))
				regparam.devicetype = "URIOS";
			else if (strstr(sip->sip_user_agent->g_string, "AND"))
				regparam.devicetype = "URAND";
			else if (strstr(sip->sip_user_agent->g_string, "WEB"))
			{
				regparam.devicetype = "URWEB";
			}
			else if (strstr(sip->sip_user_agent->g_string, "CCAAS"))
			{
				regparam.devicetype = "CCAAS";
			}
			else if (strstr(sip->sip_user_agent->g_string, "TPI"))
			{
				regparam.devicetype = "URWEB";
			}
		}
	}
	else
	{
		if (sip->sip_user_agent && sip->sip_user_agent->g_string)
		{
			if (strstr(sip->sip_user_agent->g_string, "URAPP")) 
				regparam.devicetype = "URAPP";
			else if (strstr(sip->sip_user_agent->g_string, "URWEB"))
			{
				regparam.devicetype = "URWEB";
			}
			else if (strstr(sip->sip_user_agent->g_string, "CCAAS"))
			{
				regparam.devicetype = "CCAAS";
			}
			else
			{
				regparam.devicetype = "URDESK";
				if(m_pSipHndlrParams->m_isSbcEnabled && sip->sip_contact->m_url && sip->sip_contact->m_url->url_host)
					regparam.ipaddress = sip->sip_contact->m_url->url_host;
			}

			if (strstr(sip->sip_user_agent->g_string, "IOS"))
				regparam.apptype = "IOS";
			else if (strstr(sip->sip_user_agent->g_string, "AND"))
				regparam.apptype = "AND";
			else if (strstr(sip->sip_user_agent->g_string, "URWEB:WEB"))
				regparam.apptype = "WEB";
			else if (strstr(sip->sip_user_agent->g_string, "CCAAS:BROWSER"))
				regparam.apptype = "BROWSER";
			else if (strstr(sip->sip_user_agent->g_string, "URWEB:TPI"))
				regparam.apptype = "TPI";
			else if (strstr(sip->sip_user_agent->g_string, "CCAAS:TPI"))
				regparam.apptype = "TPI";
		}
	}

    //setting transport type
	if (sip->sip_via)
	{
		if (strcmp(sip->sip_via->v_protocol, "SIP/2.0/TCP") == 0)
			regparam.transporttype = "tcp";
		else if (strcmp(sip->sip_via->v_protocol, "SIP/2.0/UDP") == 0)
			regparam.transporttype = "udp";
		else if (strcmp(sip->sip_via->v_protocol, "SIP/2.0/TLS") == 0)
			regparam.transporttype = "tls";
		else if (strcmp(sip->sip_via->v_protocol, "SIP/2.0/WSS") == 0)
			regparam.transporttype = "wss";
		else
			regparam.transporttype = "udp";
	}

	//setting cseq
    regparam.cseq = sip->sip_cseq->cs_seq;

	// setting expires value
	if (sip->sip_expires)
	{ 
		regparam.expries = (int)sip->sip_expires->ex_delta;
	}
	else if (sip->sip_contact && sip->sip_contact->m_expires)
	{
		regparam.expries = atoi(sip->sip_contact->m_expires);
	}
	else
		//TODO:what is the correct responce if expires header missing
		regparam.expries = 1800;//The default value is 1800 seconds (30 minutes) as per RFC 3261.

#if 1 // storing push_id to send update db for iOS
	for (sip_unknown_t *h = sip->sip_unknown; h != nullptr; h = h->un_next)
	{
		if (strcmp(h->un_name, "ios_pushid") == 0)
		{
			regparam.push_id = h->un_value;
			break;
		}
	}
#endif

	//storing sip_t into regsip pointer
	regparam.regsip = (void *)sip;

	sip_unknown_t *pUnknown = sip->sip_unknown;
#define UID "iuid"
	while (pUnknown)
	{
		if (!strcmp(pUnknown->un_name, UID))
		{
			regparam.message = pUnknown->un_value;
			break;
		}
		pUnknown = pUnknown->un_next;
	}
	return true;
}

xGateSofiaobj *SofiaSiphandler::CreateSofiaOptionsobj(std::string callid, nua_handle_t *nh,
													  nua_magic_t *magic, sip_t *siptag)
{

	XGLOG_FUNCTION_EN_EX("");

	xGateSofiaobj *sofiaobj = new xGateSofiaobj(); // create new Dialog
	if (!sofiaobj)
	{
		XGLOG_ERROR("SofiaSiphandler::CreateSofiaOptionsobj failed to create xGateSofiaobj for '%s' !", callid.c_str());
		return nullptr;
	}

	char uniqid[100] = "\0";
	snprintf(uniqid, 100,"%s%s%s", siptag->sip_from->a_url->url_user, siptag->sip_to->a_url->url_user, siptag->sip_call_id->i_id);
	sofiaobj->uid = callid;
	XGLOG_INFO("SofiaSiphandler::CreateSofiaOptionsobj(%s)", sofiaobj->uid.c_str());
	sofiaobj->sofianh = (void *)nh;	   // Transaction handle
	sofiaobj->siptag = (void *)siptag; // Sip tag struct information
	sofiaobj->sofiamh = (void *)magic; // Application context

	nua_saved_event_t *pSavedEvent = CreateSavedEvent();
	sofiaobj->sofiahmsg = (void *)pSavedEvent;

	if (siptag && siptag->sip_via && siptag->sip_via->v_host)
	{
		string sIpAddress("");
		sIpAddress = siptag->sip_via->v_host;
		if (ip_validator(sIpAddress) == 1)
			sofiaobj->callLegIpAddrFamily = IP_ADDR_V4;
		else
			sofiaobj->callLegIpAddrFamily = IP_ADDR_V6;
	}

	if (ValidateSipMsgPointer(siptag->sip_contact->m_url->url_params))
	{
		sofiaobj->siptransport = siptag->sip_contact->m_url->url_params;
		sofiaobj->cp.csTransport = siptag->sip_contact->m_url->url_params;
		sofiaobj->cp.cnCalled = siptag->sip_contact->m_url->url_user; // added for multithreaded-ippbx
	}

	if (siptag->sip_from && siptag->sip_from->a_url && siptag->sip_from->a_url->url_user)
		sofiaobj->cp.cnCalling = siptag->sip_from->a_url->url_user;

	if (siptag && siptag->sip_user_agent && siptag->sip_user_agent->g_string)
		sofiaobj->cp.m_AddrRecords.m_strDeviceType = siptag->sip_user_agent->g_string;

	sofiaobj->SofiaLegB = new xGateSofiaobj();
	sofiaobj->SofiaLegB->xGateCopyPeerInfo(*sofiaobj);
	SofiaBindhandle(nh, (nua_hmagic_t *)sofiaobj);

	return sofiaobj;
}
/*
 *Read sip_authorization_t header
 */
void SofiaSiphandler::SofiaReadAuthHeader(sip_authorization_t *ptAuthorization, AuthHeader &authHeader)
{
	if(!ptAuthorization)
	{
		XGLOG_INFO("SofiaSiphandler::SofiaReadAuthHeader sip_authorization_t is nullptr");
		return;
	}
	auth_response_t ar[1] = {{0}};
	auth_get_params(&m_homeMemory, ptAuthorization->au_params,
					"username=", &ar->ar_username,
					"realm=", &ar->ar_realm,
					"nonce=", &ar->ar_nonce,
					"uri=", &ar->ar_uri);

	authHeader.m_sUsername = ar->ar_username;
	authHeader.m_sRealm = ar->ar_realm;
	authHeader.m_sNonce = ar->ar_nonce;
	authHeader.m_sURI = ar->ar_uri;
}

bool SofiaSiphandler::SetcallInfoParameter(callInfoParameter &cp, sip_t const *sip, tagi_t tags[])
{
	char _temp[256];

	if (ValidateSipMsgPointer(sip->sip_from->a_url->url_host))
	{
		cp.routeAddr = sip->sip_from->a_url->url_host;
	}
	cp.csaCalled = "";
	cp.csaCalled += sip->sip_request->rq_url->url_host;
	cp.cnCalled = sip->sip_to->a_url->url_user;


	// check paid service for calling party
	// TODO Need to check required or any  hard code value need
	sip_p_asserted_identity_t *Paidurl = sip_p_asserted_identity(sip);

	if (Paidurl && Paidurl->paid_url)
	{
		cp.csaCalling += Paidurl->paid_url->url_host;
		if (Paidurl->paid_url->url_port)
		{
			cp.csaCalling += ":";
			cp.csaCalling += Paidurl->paid_url->url_port;
		}
		cp.cnCalling = Paidurl->paid_url->url_user;
	}
	else
	{
		cp.csaCalling += sip->sip_from->a_url->url_host; // TODO: We have enable this for live call
		if (sip->sip_from->a_url->url_port)
		{
			cp.csaCalling += ":";
			cp.csaCalling += sip->sip_from->a_url->url_port;
		}
		cp.cnCalling = sip->sip_from->a_url->url_user;
	}
	return true;
}

// TODO::Need to clean up below function after multiple review -Siva
nua_handle_t *SofiaSiphandler::CreateSofiaDialhandle(callInfoParameter cp, sip_t const *sip,
													 std::string &sTransProto, std::string &sRouteIP,
													 std::string &sContact,std::string call_info = "")
{
	XGLOG_FUNCTION_EN_EX("");

	nua_handle_t *nhob = nullptr;
	std::string sIPAddress("");
	std::string sSrc("");
	std::string sDest("");
	std::string sRoute("");
	std::string sPxURL("");
	std::string sPAI("");
	std::string sFromNumber("");
	std::string sFromIpAddr("");

	try
	{
		/*AOR Cache Requirement Start*/

		bool isIPV6 = false;
		bool isServerCall = false;

		if(isPBXCall(sip))
		{
			isServerCall = true;
		}

		if (isServerCall) 
		{
			if (sip && sip->sip_to && sip->sip_to->a_url && sip->sip_to->a_url->url_host)
			{
				if (ip_validator(sip->sip_to->a_url->url_host) == 0)
					isIPV6 = true;
			}
		}

		if(isIPV6)
		{
			sIPAddress =  "[" + m_pSipHndlrParams->m_sIPv6Add + "]";
		}
		else 
		{
			sIPAddress = m_pSipHndlrParams->m_sIPv4Add.c_str();
		}

		if (sip->sip_from && sip->sip_from->a_url && sip->sip_from->a_url->url_host)
		{
			sFromIpAddr = sip->sip_from->a_url->url_host;
		}

		std::string orgcallid("");
		std::string groupcall("");
        std::string groupCallCount("");
		std::string iuid("");
		std::string t_URAVConferenceRoomID("");
		std::string t_URAVConferenceUserType("");
		std::string t_URAVSource("");
		std::string t_URConferenceConId("");
		std::string t_URAVConfAor("");
		std::string strTransporttype("udp");//default it is udp
	    std::string strCcaasSessionId("");
	    std::string strCcaasFeatureReq("");
        std::string strCcaasCallerType("");
        std::string strCcaasTransferType("");
	    std::string strCallFeatureType("");
		std::string strCallInfo("");
		std::string strExtension("");
		std::string strTrunkIP("");
		std::string strIsCrmUser("");

		std::string t_confConIdforAor("");
		bool b_t_dialoutFlag(false);
		bool b_t_URAVConfAor(false);


		//TODO ::send same sip_unknown no need to parse the header 
		for (sip_unknown_t *h = sip->sip_unknown; h != nullptr; h = h->un_next)
		{
			if (strcmp(h->un_name, "Org-Call-ID") == 0)
			{
				orgcallid = "Org-Call-ID:";
				orgcallid += h->un_value;
			}

			if (strcmp(h->un_name, "Group-Call") == 0)
			{
				groupcall = "Group-Call:";
				groupcall += h->un_value;
				XGLOG_INFO("SofiaSiphandler::CreateSofiaDialhandle Group-Call:%s", groupcall.c_str());
			}

			if (strcmp(h->un_name, "Group-Call-Count") == 0)
			{
				groupCallCount = "Group-Call-Count:";
				groupCallCount += h->un_value;
				XGLOG_INFO("SofiaSiphandler::CreateSofiaDialhandle Group-Call-Count:%s", groupCallCount.c_str());
			}
 
			if (strcmp(h->un_name, "iuid") == 0)
			{
				iuid = "iuid:";
				iuid += h->un_value;
			}

			if (strcmp(h->un_name, "TRANS-PROTO") == 0)
			{
				// strTransporttype = convert_uppercase_to_lowercase(h->un_value);
				strTransporttype = h->un_value;
			}

			if (strcmp(h->un_name, "UR-AV-ConferenceRoom-ID") == 0)
			{
				t_URAVConferenceRoomID = "UR-AV-ConferenceRoom-ID:";
				t_URAVConferenceRoomID += h->un_value;
				XGLOG_INFO("SofiaSiphandler::CreateSofiaDialhandle UR-AV-ConferenceRoom-ID:%s", t_URAVConferenceRoomID.c_str());
			}

			if (strcmp(h->un_name, "UR-AV-Conference-UserType") == 0)
			{
				t_URAVConferenceUserType = "UR-AV-Conference-UserType:";
				t_URAVConferenceUserType += h->un_value;
				XGLOG_INFO("SofiaSiphandler::CreateSofiaDialhandle UR-AV-Conference-UserType:%s", t_URAVConferenceUserType.c_str());
			}

			if (strcmp(h->un_name, "UR-AV-Source") == 0)
			{
				t_URAVSource = "UR-AV-Source:";
				t_URAVSource += h->un_value;
				b_t_dialoutFlag = true;
				XGLOG_INFO("SofiaSiphandler::CreateSofiaDialhandle UR-AV-Source:%s", t_URAVSource.c_str());
			}

			if (strcmp(h->un_name, "UR-AV-ConferenceConn-ID") == 0)
			{
				t_URConferenceConId = "UR-AV-ConferenceConn-ID:";
				t_URConferenceConId += h->un_value;
				XGLOG_INFO("SofiaSiphandler::CreateSofiaDialhandle UR-AV-ConferenceConn-ID:%s", t_URConferenceConId.c_str());

				std::string aorRecord = "";
				aorRecord += cp.m_AddrRecords.m_strProxyusername;
				aorRecord += "@";
				aorRecord += cp.m_AddrRecords.m_strContactaddress;
				aorRecord += ";";
				aorRecord += cp.m_AddrRecords.m_strIpaddress;
				aorRecord += ";";
				aorRecord += cp.m_AddrRecords.m_strDeviceType;
				aorRecord += ";";

				t_URAVConfAor = "UR-AV-AOR:";
				t_URAVConfAor += aorRecord.c_str();
				XGLOG_INFO("SofiaSiphandler::CreateSofiaDialhandle UR-AV-AOR = (%s)", t_URAVConfAor.c_str());
			}

			if (strcmp(h->un_name, "UR-AV-Conference-DstConn-ID") == 0)
			{
				t_confConIdforAor = h->un_value;
				XGLOG_INFO("SofiaSiphandler::CreateSofiaDialhandle UR-AV-Conference-DstConn-ID:%s", t_confConIdforAor.c_str());
			}
			if (strcmp(h->un_name, "CCAAS-SESSION-ID") == 0)
			{
				strCcaasSessionId = "CCAAS-SESSION-ID:";
				strCcaasSessionId += h->un_value;
				XGLOG_INFO("SofiaSiphandler::CreateSofiaDialhandle CCAAS-SESSION-ID:%s", strCcaasSessionId.c_str());
			}

			if (strcmp(h->un_name, "CCAAS-FEATURE-REQ") == 0)
			{
				strCcaasFeatureReq = "CCAAS-FEATURE-REQ:";
				strCcaasFeatureReq += h->un_value;
				XGLOG_INFO("SofiaSiphandler::CreateSofiaDialhandle CCAAS-FEATURE-REQ:%s", strCcaasFeatureReq.c_str());
			}

			if (strcmp(h->un_name, "CCAAS-CALLER-TYPE") == 0)
			{
				strCcaasCallerType = "CCAAS-CALLER-TYPE:";
				strCcaasCallerType += h->un_value;
				XGLOG_INFO("SofiaSiphandler::CreateSofiaDialhandle CCAAS-CALLER-TYPE:%s", strCcaasCallerType.c_str());
			}

			if (strcmp(h->un_name, "CCAAS-TRANSFER-TYPE") == 0)
			{
				strCcaasCallerType = "CCAAS-TRANSFER-TYPE:";
				strCcaasCallerType += h->un_value;
				XGLOG_INFO("SofiaSiphandler::CreateSofiaDialhandle CCAAS-TRANSFER-TYPE:%s", strCcaasTransferType.c_str());
			}

			if (strcmp(h->un_name, "Call-Feature-Type") == 0)
			{
				strCallFeatureType = "Call-Feature-Type:";
				strCallFeatureType += h->un_value;
				XGLOG_INFO("SofiaSiphandler::CreateSofiaDialhandle Call-Feature-Type:%s", strCallFeatureType.c_str());
			}
			if (strcmp(h->un_name, "IS-CRM-USER") == 0)
			{
				strIsCrmUser = "IS-CRM-USER:";
				strIsCrmUser += h->un_value;
				XGLOG_INFO("SofiaSiphandler::CreateSofiaDialhandle IS-CRM-USER:%s", strIsCrmUser.c_str());
			}
			// XGLOG_INFO("Unknown  header (%s): %s", h->un_name, h->un_value);
		}

		//Call-Info Unknown header
		if(!call_info.empty())
		{
			strCallInfo = "PBX-Call-Info:" + call_info;
		}
		cout << "CALL_INFO********" << strCallInfo << endl;
		
		// Send Trunk Ip to IPPBX for processing as trunk call
		auto trunk_map = CONFIGSERVICE->m_lightWeightWSSProfile.m_TrunkMap;
		if (trunk_map.find(sFromIpAddr.c_str()) != trunk_map.end())
		{
			strTrunkIP = "SIP-TRUNK-IP:";
			strTrunkIP += sFromIpAddr;
		}

		std::string strContactAddr("");
		std::string strHostIp("");
		std::string strHostPort("");
		if ((b_t_dialoutFlag == true) || (strstr(sip->sip_via->v_protocol, "SIP/2.0/TCP")) || (strstr(sip->sip_via->v_protocol, "SIP/2.0/UDP")))
		{

			if (t_confConIdforAor != "")
			{
				map<std::string, ClientAOR *>::iterator itr;

				itr = this->m_mapConfUserAor.find(t_confConIdforAor);
				if (itr != this->m_mapConfUserAor.end())
				{
					ClientAOR *t_clientAoR = itr->second;
					strContactAddr = t_clientAoR->m_contactAddr.c_str();
					std::string delimiter = ":";
					string::size_type ndlimpos = strContactAddr.find_last_of(delimiter);

					if (ndlimpos != string::npos)
					{
						strHostIp = strContactAddr.substr(0, ndlimpos);
						strHostPort = strContactAddr.substr(ndlimpos + 1);
					}
					strTransporttype = t_clientAoR->m_transport;

					std::cout << "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&" << std::endl;
					std::cout << "CONN-ID : " << t_confConIdforAor.c_str() << std::endl;
					std::cout << "SETTING CONTACT ADDR FROM AOR CACHE : " << strContactAddr.c_str() << std::endl;
					std::cout << "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&" << std::endl;
					XGLOG_INFO("SofiaSiphandler::CreateSofiaDialhandle AOR From Cache Connection ID(%s), Client IP(%s), Client Port(%s)",
							   t_confConIdforAor.c_str(),
							   strHostIp.c_str(),
							   strHostPort.c_str());
				}
				else
				{
					XGLOG_INFO("SofiaSiphandler::CreateSofiaDialhandle Client AOR details are empty");
				}
			}
		}

		if (!strHostIp.empty())
			sip->sip_to->a_url->url_host = strHostIp.c_str();
		if (!strHostPort.empty())
			sip->sip_to->a_url->url_port = strHostPort.c_str();
		if (!strTransporttype.empty())
			sTransProto = strTransporttype.c_str();

		if (cp.isInboundPstnCall)
		{
			sip->sip_from->a_url->url_host = sIPAddress.c_str();
			sip->sip_to->a_url->url_host = sRouteIP.c_str();
		}

		if(cp.isMsTeamsCall)
		{
			auto[bAvailable,sNumber,sExt] = extractCliandExt(sip->sip_from->a_url->url_user);
			if(bAvailable)
			{
				sFromNumber = sNumber.c_str();
				sip->sip_from->a_url->url_user = sFromNumber.c_str();
				strExtension = "Ext:";
				strExtension += sExt.c_str();
			}
			strCallFeatureType = "Call-Feature-Type:";
            strCallFeatureType += "MS-Teams";
		}

		/*AOR Cache Requirement End*/

		//Encoding Paid URL 
		sip_p_asserted_identity_t *paidurl = sip_p_asserted_identity(sip);
		if(paidurl)
			sPAI = EncodeSipHeader(sip->sip_from->a_url, cp, SIPVALIDATE::PAI, sIPAddress , sTransProto,false,paidurl);

		if (!isServerCall)
		{
			// Encoding Route Header
			sRoute = EncodeSipHeader(sip->sip_to->a_url, cp, SIPVALIDATE::ROUTE, sRouteIP, sTransProto);
		}

		// Encoding From Header
		sSrc = EncodeSipHeader(sip->sip_from->a_url, cp, SIPVALIDATE::FROM, sIPAddress, sTransProto, isServerCall);
		// Encoding To Header
		sDest = EncodeSipHeader(sip->sip_to->a_url, cp, SIPVALIDATE::TO, sIPAddress, sTransProto,isServerCall);
		// Encoding Contact Header
		sContact = EncodeSipHeader(sip->sip_from->a_url, cp, SIPVALIDATE::CONTACT, sIPAddress, sTransProto);

		/*Fill remoteHeaderInfo*/
		std::string remoteHeaderInfo("");
		remoteHeaderInfo = "ExtraRemote-Info:";
		std::string extraHeaderCallId("");
		std::string fromTag("");

		extraHeaderCallId = "%RemoteCallId%";
		extraHeaderCallId += sip->sip_call_id->i_id;
		fromTag = "%From-tag%";
		fromTag += sip->sip_from->a_tag;

		remoteHeaderInfo += extraHeaderCallId + fromTag;
		//cout << "remoteHeaderInfo: " << remoteHeaderInfo << endl;


#if 0//Siva ,May be feature use 
		if (strstr(sip->sip_via->v_protocol, "SIP/2.0/WSS") || strstr(sip->sip_via->v_protocol, "SIP/2.0/TLS"))
		{

			// std::map<std::string, std::string> t_inboundRouteAddrMap = CONFIGSERVICE->m_lightWeightWSSProfile.m_inboundRouteAddrMap;

			std::string to_host_url = sip->sip_to->a_url->url_host;
			std::string bridge_ip = "";
			std::string w_bridge_ip = "";

			route = "<sip:";

			if (cp.is302messagearrived == true || cp.isServerFailure == true)
			{
				XGLOG_INFO("SofiaSiphandler::CreateSofiaDialhandle 302 message arrived or Server Failure ");
				cp.is302messagearrived = false;
				cp.isServerFailure = false;
				route += cp.forwardingmgcrouteIp;
				sRouteIP = cp.forwardingmgcrouteIp;
			}
			else
			{

				bridge_ip = CONFIGSERVICE->m_lightWeightWSSProfile.getInboundRouteIp(to_host_url);

				if (!bridge_ip.empty())
				{
					routeBridgeServerIp = bridge_ip;
					XGLOG_INFO("SofiaSiphandler::CreateSofiaDialhandle Route addr for to_host_url(%s) is (%s)", to_host_url.c_str(), routeBridgeServerIp.c_str());
				}
				else if (true == validateDomainWithWildcardRouteAddrSerialRouting(to_host_url, &w_bridge_ip))
				{
					if (!w_bridge_ip.empty())
					{
						routeBridgeServerIp = w_bridge_ip;
						XGLOG_INFO("SofiaSiphandler::CreateSofiaDialhandle Route addr from Wildcard for to_host_url(%s) is (%s)", to_host_url.c_str(), routeBridgeServerIp.c_str());
					}
					else
					{
						XGLOG_ERROR("SofiaSiphandler::CreateSofiaDialhandle NO Route addr found for to_host_url(%s)", to_host_url.c_str());
						routeBridgeServerIp = CONFIGSERVICE->m_lightWeightWSSProfile.m_routeBridgeServerIp;
						XGLOG_ERROR("SofiaSiphandler::CreateSofiaDialhandle using defualt route address!");
					}
				}
				else
				{
					XGLOG_ERROR("SofiaSiphandler::CreateSofiaDialhandle NO Route addr found for to_host_url(%s)", to_host_url.c_str());
					routeBridgeServerIp = CONFIGSERVICE->m_lightWeightWSSProfile.m_routeBridgeServerIp;
					XGLOG_ERROR("SofiaSiphandler::CreateSofiaDialhandle using defualt route address!");
				}

				XGLOG_INFO("Bridge server IP Configuration is set to = (%s)", routeBridgeServerIp.c_str());
				route += routeBridgeServerIp.c_str();
				sRouteIP = routeBridgeServerIp.c_str();


			}
			route +=sRouteIP.c_str();
			route += ":5060;lr>";
		}

		cout << route.c_str() << endl;
		XGLOG_INFO("Route = (%s)", route.c_str());
#endif

		nhob = nua_handle(GetAppContext(), nullptr,
						  SIPTAG_TO_STR(sDest.c_str()),
						  SIPTAG_FROM_STR(sSrc.c_str()),
						  SIPTAG_CONTACT_STR(sContact.c_str()),
						  TAG_IF(!sRoute.empty(), SIPTAG_ROUTE_STR(sRoute.c_str())),
						  TAG_IF(sip->sip_proxy_authorization, SIPTAG_PROXY_AUTHORIZATION(sip->sip_proxy_authorization)),
						  TAG_IF(sip->sip_replaces, SIPTAG_REPLACES(sip->sip_replaces)),
						  TAG_IF(!orgcallid.empty(), SIPTAG_HEADER_STR(orgcallid.c_str())),
						  TAG_IF(!groupcall.empty(), SIPTAG_HEADER_STR(groupcall.c_str())),
						  TAG_IF(!groupCallCount.empty(), SIPTAG_HEADER_STR(groupCallCount.c_str())),
						  TAG_IF(!sPAI.empty(), SIPTAG_P_ASSERTED_IDENTITY_STR(sPAI.c_str())),
						  TAG_IF(!iuid.empty(), SIPTAG_HEADER_STR(iuid.c_str())),
						  TAG_IF(!remoteHeaderInfo.empty(), SIPTAG_HEADER_STR(remoteHeaderInfo.c_str())),
						  TAG_IF(!t_URAVConferenceRoomID.empty(), SIPTAG_HEADER_STR(t_URAVConferenceRoomID.c_str())),
						  TAG_IF(!t_URAVConferenceUserType.empty(), SIPTAG_HEADER_STR(t_URAVConferenceUserType.c_str())),
						  TAG_IF(!t_URAVSource.empty(), SIPTAG_HEADER_STR(t_URAVSource.c_str())),
						  TAG_IF(!t_URConferenceConId.empty(), SIPTAG_HEADER_STR(t_URConferenceConId.c_str())),
						  TAG_IF(!t_URAVConfAor.empty(), SIPTAG_HEADER_STR(t_URAVConfAor.c_str())), /*  SIPTAG_PAYLOAD_STR(cp.sdpStr.c_str()),\*/
						  TAG_IF(!strCcaasSessionId.empty(), SIPTAG_HEADER_STR(strCcaasSessionId.c_str())),
						  TAG_IF(!strCcaasFeatureReq.empty(), SIPTAG_HEADER_STR(strCcaasFeatureReq.c_str())),
						  TAG_IF(!strCcaasCallerType.empty(), SIPTAG_HEADER_STR(strCcaasCallerType.c_str())),
						  TAG_IF(!strCcaasTransferType.empty(), SIPTAG_HEADER_STR(strCcaasTransferType.c_str())),
						  TAG_IF(!strCallFeatureType.empty(), SIPTAG_HEADER_STR(strCallFeatureType.c_str())),
						  TAG_IF(!strCallInfo.empty(), SIPTAG_HEADER_STR(strCallInfo.c_str())),
						  TAG_IF(!strExtension.empty(), SIPTAG_HEADER_STR(strExtension.c_str())),
						  TAG_IF(!strTrunkIP.empty(), SIPTAG_HEADER_STR(strTrunkIP.c_str())),
						  TAG_IF(!strIsCrmUser.empty(), SIPTAG_HEADER_STR(strIsCrmUser.c_str())),
						  NUTAG_AUTOANSWER(0),
						  TAG_END());

		//Encoding Proxy URL
		if (isServerCall)
			sPxURL = EncodeSipHeader(sip->sip_to->a_url, cp, SIPVALIDATE::PROXY_ROUTE, sIPAddress, sTransProto, isServerCall);
		else
			sPxURL = EncodeSipHeader(sip->sip_to->a_url, cp, SIPVALIDATE::PROXY_ROUTE, sRouteIP, sTransProto, isServerCall);

		if(isServerCall)
			nua_set_hparams(nhob, NUTAG_PROXY((url_string_t *)sPxURL.c_str()),
								  TAG_IF(sip->sip_user_agent, SIPTAG_USER_AGENT_STR(XGATE_USER_AGENT)),TAG_END());
		else
			nua_set_hparams(nhob, NUTAG_PROXY((url_string_t *)sPxURL.c_str()),
								  TAG_IF(sip->sip_user_agent, SIPTAG_USER_AGENT(sip->sip_user_agent)), TAG_END());
	}
	catch (...)
	{
		XGLOG_WARN("SofiaSiphandler::CreateSofiaDialhandle Exception ");
		return nullptr;
	}
	XGLOG_WARN("SofiaSiphandler::CreateSofiaDialhandle Completed ");
	return nhob;
}

/*
 *Sending Message Request to IPPBX with registerd port and IP
 */
void SofiaSiphandler::SofiaSendMessageRequest(xGateSofiaSipRegobj *RegObj)
{
	if (!RegObj)
	{
		XGLOG_ERROR("SofiaSiphandler::SofiaSendMessageRequest RegObj is nullptr");
		return;
	}

	if (RegObj->message.empty())
	{
		XGLOG_ERROR("SofiaSiphandler::SofiaSendMessageRequest iuid empty in Registeration Request ,\
														so no need to send MESSAGE request to PBX for callid %s",
					RegObj->callid.c_str());
		return;
	}

	std::string sFrom(""), sTo(""), sContact(""), sIuid(""), sPbxIP(""), sRouteIP(""), sAOR(""), sDeviceType(""), sIuidValue(""), sTransProto("");
	std::string uid = RegObj->message;

	std::string dlim("_");
	std::string::size_type nDelimPos = uid.find(dlim);
	if (nDelimPos != string::npos)
	{
		sIuid = uid.substr(0, nDelimPos);
		sPbxIP = uid.substr(nDelimPos + 1);
	}

	if (sPbxIP.empty() || sIuid.empty())
	{
		XGLOG_ERROR("SofiaSiphandler::SofiaSendMessageRequest sPbxIP or sIuid is empty for %s so sending Message Request failed", RegObj->callid.c_str());
		return;
	}

	// from string
	sFrom.append(RegObj->proxyusername.c_str());
	sFrom.append("<sip:");
	sFrom.append(RegObj->proxyusername.c_str());
	sFrom.append("@");
	sFrom.append(m_pSipHndlrParams->m_sIPv4Add.c_str());
	sFrom.append(":");
	sFrom.append(m_pSipHndlrParams->m_sSipPort.c_str());
	sFrom.append(">");
	// to string
	sTo.append(RegObj->proxyusername.c_str());
	sTo.append("<sip:");
	sTo.append(RegObj->proxyusername.c_str());
	sTo.append("@");
	sTo.append(sPbxIP.c_str());
	sTo.append(":");
	sTo.append(m_pSipHndlrParams->m_sSipPort.c_str());
	sTo.append(">");
#if 0 // Siva::Feature use for MESSAGE request no neeed to give contact 
	//contact string
	sContact.append(RegObj->proxyusername.c_str());
	sContact.append("<sip:");
	sContact.append(RegObj->proxyusername.c_str());
	sContact.append("@");
	sContact.append(CONFIGSERVICE->m_lightWeightWSSProfile.m_contactWssIp.c_str());
	sContact.append(":");
	sContact..append(m_pSipHndlrParams->m_sSipPort.c_str());
	sContact.append(";transport=udp>");
#endif

	// AOR string
	sAOR.append("AOR:");
	sAOR.append(RegObj->contactaddress.c_str());

	// Device Type string
	sDeviceType.append(RegObj->devicetype.c_str());
	sDeviceType.append(":");
	sDeviceType.append(RegObj->apptype.c_str());

	// transport type string
	sTransProto.append("TRANS-PROTO:");
	sTransProto.append(RegObj->transporttype.c_str());

	// iuid string
	sIuidValue.append("IUID:");
	sIuidValue.append(sIuid.c_str());
	// route_ip string
	sRouteIP.append("ROUTE-IP:");
	sRouteIP.append(RegObj->ipaddress.c_str());

	nua_handle_t *pMsgHandle = nua_handle(GetAppContext(), nullptr,
										  SIPTAG_TO_STR(sTo.c_str()),
										  SIPTAG_FROM_STR(sFrom.c_str()),
										  SIPTAG_CALL_INFO_STR(sDeviceType.c_str()),
										  SIPTAG_HEADER_STR(sIuidValue.c_str()),
										  SIPTAG_HEADER_STR(sAOR.c_str()),
										  SIPTAG_HEADER_STR(sTransProto.c_str()),
										  SIPTAG_PAYLOAD_STR(sIuid.c_str()),
										  SIPTAG_HEADER_STR(sRouteIP.c_str()), TAG_END());

	if (pMsgHandle != nullptr)
	{
		nua_message(pMsgHandle, TAG_END());
		XGLOG_INFO("SofiaSiphandler::SofiaSendMessageRequest MESSAGE send to PBX callid (%s)", RegObj->callid.c_str());
	}
	else
	{
		XGLOG_ERROR("SofiaSiphandler::SofiaSendMessageRequest Send Message Request Failed  callid (%s)", RegObj->callid.c_str());
	}
}

void SofiaSiphandler::SofiaSendOptionsRequest(xGateSofiaobj *sobj)
{
	if (!sobj)
	{
		XGLOG_ERROR("SofiaSiphandler::SofiaSendOptionsRequest sobj is nullptr");
		return;
	}

	// nua_saved_event_t *pSavedEvent = CreateSavedEvent();
	// sobj->sofiahmsg = (void *)pSavedEvent;
	std::string sFrom(""), sTo(""), sContact("");

	// from string
	sFrom.append(sobj->m_sCallingNumber.c_str());
	sFrom.append("<sip:");
	sFrom.append(sobj->m_sCallingNumber.c_str());
	sFrom.append("@");
	sFrom.append(m_pSipHndlrParams->m_sIPv4Add.c_str());
	sFrom.append(":");
	sFrom.append(m_pSipHndlrParams->m_sSipPort.c_str());
	sFrom.append(">");

	// to string
	sTo.append(sobj->m_sCallingNumber.c_str());
	sTo.append("<sip:");
	sTo.append(sobj->m_sCallingNumber.c_str());
	sTo.append("@");
	sTo.append(sobj->m_sRouteIP.c_str());
	sTo.append(":");
	sTo.append(m_pSipHndlrParams->m_sSipPort.c_str());
	sTo.append(">");

	// contact string
	sContact.append(sobj->m_sCallingNumber.c_str());
	sContact.append("<sip:");
	sContact.append(sobj->m_sCallingNumber.c_str());
	sContact.append("@");
	sContact.append(m_pSipHndlrParams->m_sIPv4Add.c_str());
	sContact.append(":");
	sContact.append(m_pSipHndlrParams->m_sSipPort.c_str());
	sContact.append(";transport=udp>");

	nua_handle_t *pOptionsHandle = nua_handle(GetAppContext(), nullptr,
											  SIPTAG_TO_STR(sTo.c_str()),
											  SIPTAG_FROM_STR(sFrom.c_str()),
											  SIPTAG_CALL_ID_STR(sobj->callid.c_str()),
											  SIPTAG_CONTACT_STR(sContact.c_str()),
											  TAG_END());
	SofiaBindhandle(pOptionsHandle, (nua_hmagic_t *)sobj);

	if (pOptionsHandle != nullptr)
	{
		nua_options(pOptionsHandle, TAG_END());
		XGLOG_INFO("SofiaSiphandler::SofiaSendOptionsRequest OPTIONS  success callid(%s) TrunkId(%d) and IpAddress(%s)", sobj->callid.c_str(),sobj->m_iTrunkId,sobj->m_sRouteIP.c_str());
	}
	else
	{
		XGLOG_ERROR("SofiaSiphandler::SofiaSendOptionsRequest OPTIONS failed callid(%s) TrunkId(%d) and IpAddress(%s)", sobj->callid.c_str(),sobj->m_iTrunkId,sobj->m_sRouteIP.c_str());
	}

	return;
}

void SofiaSiphandler::ProcessInviteRequest(nua_handle_t *nh, sip_t const *sip, tagi_t tags[], xGateSofiaobj *sobj)
{
	XGLOG_WARN("SofiaSiphandler::ProcessInviteRequest(%s)", sobj->uid.c_str());

	// Create a New Call object Since it is new invite found
	xGateCallObj *callobj = new xGateCallObj;
	if (!callobj)
	{
		XGLOG_ERROR("SofiaSiphandler::ProcessInviteRequest failed to create xGateCallObj for '%s'!", sobj->uid.c_str());
	}

	CreateCallobjparam(callobj, sobj, nh, sip, tags);
	// Post Message
	callobj->sipeventtype = EN_INVITE_REQ;
}

/*
 *handle Client REGISTERATION Request without Auth header (sending 401 Unauthorized response)
 */
bool SofiaSiphandler::handleClientRegRequestWOAuth(nua_handle_t *nh, sip_t const *sip)
{
	XGLOG_TRACE("Registration_from(%s), domain(%s) device(%s) callid(%s) WO Auth" ,sip->sip_from->a_url->url_user,\
	                                                                        	    sip->sip_from->a_url->url_host,\
														                            (sip->sip_user_agent && sip->sip_user_agent->g_string)?sip->sip_user_agent->g_string:"device_empty",\
																	                sip->sip_call_id->i_id);
	char auth_header[624] = {};
	char buffer[512] = {};

	mod = SofiaInitAuth(m_root);
	if (mod)
	{
		nua_saved_event_t *pSavedEvent = CreateSavedEvent();
		auth_generate_digest_nonce(mod, buffer, 512, false, msg_now()); // generate the nonce
		snprintf(auth_header,624, " Digest realm=\"%s\",nonce=\"%s\"", sip->sip_to->a_url->url_host, buffer);
		nua_respond(nh, 401, "Unauthorized", SIPTAG_WWW_AUTHENTICATE_STR(auth_header), SIPTAG_SUPPORTED_STR(""),
					SIPTAG_DATE(sip->sip_date), NUTAG_WITH_THIS(m_nuaHandle), NUTAG_WITH_SAVED(pSavedEvent),
					NUTAG_M_PARAMS(sip->sip_contact->m_url->url_params), TAG_END());

		XGLOG_INFO("SofiaSiphandler handleRegRequestWOAuth(%s)-- send 401 unauthorized response", sip->sip_call_id->i_id);
		DestroySavedEvent(pSavedEvent);
		SofiaDeInitAuth(mod);
		return true;
	}
	return false;
}

/*
 *handler Client REGISTERATION Request with Auth header ,processing the request
 */
bool SofiaSiphandler::handleClientRegRequestWithAuth(nua_handle_t *nh, sip_t const *sip, nua_magic_t *magic, nua_hmagic_t *hmagic)
{

	XGLOG_TRACE("Registration_from(%s), domain(%s) device(%s) callid(%s) ",sip->sip_from->a_url->url_user,\
	                                                                       sip->sip_from->a_url->url_host,\
														                   (sip->sip_user_agent && sip->sip_user_agent->g_string)?sip->sip_user_agent->g_string:"device_empty",\
																	        sip->sip_call_id->i_id);

	// Create the Registration object to post to Registration Manager
	xGateSofiaSipRegobj *sipregobj = (xGateSofiaSipRegobj *)hmagic;

	if (!sipregobj)
	{
		sipregobj = new xGateSofiaSipRegobj();
		CreateSofiaRegobj(*sipregobj, sip);
	}
	sipregobj->Reghandle = (void *)nh;
	sipregobj->sofiamh = (void *)magic;

	/*AOR Cache Requirement Start*/
	for (sip_unknown_t *h = sip->sip_unknown; h != nullptr; h = h->un_next)
	{

		if (strcmp(h->un_name, "UR-AV-ConferenceRoom-ID") == 0)
		{
			sipregobj->URAVConfRoomID = h->un_value;
			XGLOG_INFO("SofiaSiphandler::handleClientRegRequestWithAuth UR-AV-ConferenceRoom-ID:%s", h->un_value);
		}

		if (strcmp(h->un_name, "UR-AV-ConferenceConn-ID") == 0)
		{
			sipregobj->URAVConfConnID = h->un_value;
			XGLOG_INFO("SofiaSiphandler::handleClientRegRequestWithAuth UR-AV-ConferenceConn-ID:%s", h->un_value);
		}

		if (strcmp(h->un_name, "iuid") == 0)
		{
			sipregobj->iuid = h->un_value;
			XGLOG_INFO("SofiaSiphandler::handleClientRegRequestWithAuth iuid :%s", h->un_value);
		}
	}
	
	//SofiaReadAuthHeader(sip->sip_authorization, sipregobj->m_authHeader);

	/*AOR Cache Requirement End*/
	nua_saved_event_t *RegEvent = CreateSavedEvent();
	sipregobj->sofiahmsg = (void *)RegEvent;

	SofiaBindhandle(nh, (nua_hmagic_t *)sipregobj); // bind the handle

	// post to controller for processing the request
	sipregobj->Regevent = EN_REGISTER;
	xGateCallObj *callobj = new xGateCallObj;
	callobj->m_regObj = sipregobj;
	callobj->sipeventtype = EN_REGISTRATION_REQ;
	PostMessage(callobj, XGATEPOSTTYPE::EN_PROCESS);

	return true;
}

/*
* handle Invite Request from Stack
*/
bool SofiaSiphandler::handleInviteRequest(std::string callId, nua_handle_t *nh, sip_t const *sip, nua_magic_t *magic, nua_hmagic_t *hmagic,tagi_t tags[])
{

	if(isMSTeamsCall(sip->sip_from->a_url->url_host) == true)
	{
		handleMSTeamsInviteRequest(nh, sip, tags, magic, hmagic);
		return true;
	}

	if(!isPBXCall(sip) && !sip->sip_proxy_authorization && \
						  (strlen(sip->sip_from->a_url->url_user)) >= INBOUND_CALL_MIN_LEN && \
						  (strlen(sip->sip_to->a_url->url_user)) >= INBOUND_CALL_MIN_LEN )
	{
		handleInboundPstnInviteRequest(callId, nh, sip, tags, magic, hmagic);
		return true;
	}
	if (isPBXCall(sip))
	{
		// Process server Invite Request
		handleServerInviteRequest(callId, nh, sip, tags, magic, hmagic);
	}
	else
	{
		// Process Client Invite Request
		if (sip->sip_proxy_authorization)
		{
			handleClientInviteRequestWithAuth(callId, nh, sip, magic, hmagic);
		}
		else
		{
			handleClientInviteRequestWOAuth(callId, nh, sip);
		}
	}

	return true;
}

/*
 * handle Client INVITE Request without Auth header ,so sending 407 Proxy Authentication Required
 */
bool SofiaSiphandler::handleClientInviteRequestWOAuth(std::string callId, nua_handle_t *nh, sip_t const *sip)
{
	char auth_header[624] = {};
	char buffer[512] = {};
	XGLOG_TRACE("Invite_from(%s)_to(%s) domain(%s) callid(%s) WO AUTH ",sip->sip_from->a_url->url_user,\
															  sip->sip_to->a_url->url_user,\
                                                              sip->sip_from->a_url->url_host,\
															  callId.c_str());
	mod = SofiaInitAuth(m_root);
	if (mod)
	{
		auth_generate_digest_nonce(mod, buffer, 1024, false, msg_now()); // generate the nonce
		snprintf(auth_header,624, " Digest realm=\"%s\",nonce=\"%s\"", sip->sip_to->a_url->url_host, buffer);
		nua_respond(nh, 407, "Proxy Authentication Required", SIPTAG_PROXY_AUTHENTICATE_STR(auth_header),
						SIPTAG_SUPPORTED_STR(""), SIPTAG_DATE(sip->sip_date), TAG_END());

		XGLOG_INFO("SofiaSiphandler handleInviteRequestWOAuth(%s)-- send 407 Proxy Authentication Required ", callId.c_str());
		SofiaDeInitAuth(mod);
		return true;
	}
	return false;
}
/*
 * handle Client INVITE Request with Auth header ,processing the request and posting to controller
 */
bool SofiaSiphandler::handleClientInviteRequestWithAuth(std::string callId, nua_handle_t *nh, sip_t const *sip, nua_magic_t *magic, nua_hmagic_t *hmagic)
{
	xGateSofiaobj *sobj = nullptr;
	sobj = CreateSofiaobjparam(callId, nh, magic, sip); // create and bind the sofia nua_handle
	xGateCallObj *callobj = new xGateCallObj();
	callobj->uid = sobj->uid.c_str();
	callobj->callid = sobj->callid.c_str();
	callobj->m_sCallingNumber = sobj->m_sCallingNumber.c_str();
	callobj->m_sCalledNumber = sobj->m_sCalledNumber.c_str();
	callobj->m_sDomainName = sobj->m_sDomainName.c_str();
	if (!sobj->m_sKey.empty())
		callobj->m_sKey = sobj->m_sKey.c_str();
	callobj->sipeventtype = EN_INVITE_REQ;
	PostMessage(callobj, XGATEPOSTTYPE::EN_PROCESS);
	return true;
}

/*
 * handle Server INVITE Request ,processing the request
 */
bool SofiaSiphandler::handleServerInviteRequest(std::string callId, nua_handle_t *nh, sip_t const *sip,tagi_t tags[], nua_magic_t *magic, nua_hmagic_t *hmagic)
{
	xGateSofiaobj *sobj = nullptr;
	sobj = CreateSofiaobjparam(callId, nh, magic, sip); // create and bind the sofia nua_handle
	//ProcessWssInviteRequest(nh,sip,tags,sCallid,1);
	return true;
}

/*
 * handle Inbound INVITE Request from MVNO network ,processing the request 
 */
bool SofiaSiphandler::handleInboundPstnInviteRequest(std::string callId, nua_handle_t *nh, sip_t const *sip, tagi_t tags[], nua_magic_t *magic, nua_hmagic_t *hmagic)
{
	xGateSofiaobj *sobj = nullptr;
	std::string sCallid("");
	sCallid = sip->sip_call_id->i_id;
	sobj = CreateSofiaobjparam(sCallid, nh, magic, sip,CALL_TYPE::EN_INBOUND_PSTN_CALL); // create and bind the sofia nua_handle
	xGateCallObj *callobj = new xGateCallObj();
	callobj->uid = sobj->uid.c_str();
	callobj->callid = sobj->callid.c_str();
	callobj->m_sCallingNumber = sobj->m_sCallingNumber.c_str();
	callobj->m_sCalledNumber = sobj->m_sCalledNumber.c_str();
	callobj->sipeventtype = EN_INVITE_INBOUND_PSTN_REQ;
	PostMessage(callobj, XGATEPOSTTYPE::EN_PROCESS);
	XGLOG_INFO("SofiaSiphandler::handleInboundPstnInviteRequest posted callid(%s)",callobj->uid.c_str());
	return true;
}

/*
* handle Microsoft Team call from FRAFOS ABC SBC 
*/
bool SofiaSiphandler::handleMSTeamsInviteRequest(nua_handle_t *nh, sip_t const *sip, tagi_t tags[], nua_magic_t *magic, nua_hmagic_t *hmagic)
{
	xGateSofiaobj *sobj = nullptr;
	std::string sCallid("");
	sCallid = sip->sip_call_id->i_id;
	sobj = CreateSofiaobjparam(sCallid, nh, magic, sip,CALL_TYPE::EN_MS_TEAMS_CALL); // create and bind the sofia nua_handle
	xGateCallObj *callobj = new xGateCallObj();
	callobj->uid = sobj->uid.c_str();
	callobj->callid = sobj->callid.c_str();
	callobj->m_sCallingNumber = sobj->m_sCallingNumber.c_str();
	callobj->m_sCalledNumber = sobj->m_sCalledNumber.c_str();
	callobj->m_sDomainName = sobj->m_sDomainName.c_str();
	callobj->m_sCallingExt = sobj->m_sCallingExt.c_str();
	callobj->sipeventtype = EN_INVITE_MS_TEAMS_REQ;
	PostMessage(callobj, XGATEPOSTTYPE::EN_PROCESS);
	XGLOG_INFO("SofiaSiphandler::handleMSTeamsInviteRequest posted callid(%s)", callobj->uid.c_str());
	return true;
}

/*
* handle Invite response from controller ,validating the password and sending Invite route IP
*/
bool SofiaSiphandler::handleInviteRespFromCtrl(xGateSofiaobj *sobj, xGateCallObj *callObj)
{
	if (!sobj || !callObj)
	{
		XGLOG_ERROR("SofiaSiphandler::handleInviteRespFromCtrl sobj or callObj nullptr");
		return false;
	}
	sobj->m_sPassword = callObj->m_sPassword.c_str();
	sobj->m_sRouteIP  = callObj->m_sRouteIP.c_str();
	if(!callObj->m_sCallInfo.empty())
		sobj->m_sCallInfo = callObj->m_sCallInfo;
	if(VerifyAuthorisationResponce((sip_t *)sobj->siptag,sobj->m_sPassword,"INVITE") == true)
	{
		ProcessWssInviteRequest((nua_handle_t *)sobj->sofianh, (sip_t *)sobj->siptag,(tagi_t *)sobj->siptag,sobj->uid, 1);
	}
	else 
	{
		nua_respond((nua_handle_t *)sobj->sofianh,403 ,"Forbidden",TAG_END());
		XGLOG_ERROR("SofiaSiphandler::handleInviteResponseFromCtrl passsword validation failed callid(%s) ,so sending 403 Forbidden",sobj->callid.c_str());
	}

	return true;
}

/*
* handle Invite response from controller for inbound PSTN ,validating the password and sending Invite route IP
*/
bool SofiaSiphandler::handleInviteInboundPstnRespFromCtrl(xGateSofiaobj *sobj, xGateCallObj *callObj)
{
	if (!sobj || !callObj)
	{
		XGLOG_ERROR("SofiaSiphandler::handleInviteInboundPstnRespFromCtrl sobj or callObj nullptr");
		return false;
	}
	sobj->m_sRouteIP  = callObj->m_sRouteIP.c_str();
	sobj->mediastate  = EN_MED_SET;//Siva ,TODO ::Need to remove those mediastate logics
	if (!callObj->m_sCallInfo.empty())
		sobj->m_sCallInfo = callObj->m_sCallInfo;
	ProcessWssInviteRequest((nua_handle_t *)sobj->sofianh, (sip_t *)sobj->siptag,(tagi_t *)sobj->siptag,sobj->uid, 1);
	return true;
}

/*
* handle Invite response from controller for inbound PSTN ,validating the password and sending Invite route IP
*/
bool SofiaSiphandler::handleInviteMSTeamsRespFromCtrl(xGateSofiaobj *sobj, xGateCallObj *callObj)
{
	if (!sobj || !callObj)
	{
		XGLOG_ERROR("SofiaSiphandler::handleInviteMSTeamsRespFromCtrl sobj or callObj nullptr");
		return false;
	}
	sobj->m_sRouteIP  = callObj->m_sRouteIP.c_str();
	sobj->mediastate  = EN_MED_SET;//Siva ,TODO ::Need to remove those mediastate logics
	if (!callObj->m_sCallInfo.empty())
		sobj->m_sCallInfo = callObj->m_sCallInfo;
	ProcessWssInviteRequest((nua_handle_t *)sobj->sofianh, (sip_t *)sobj->siptag,(tagi_t *)sobj->siptag,sobj->uid, 1);
	return true;
}


/*
*handle Invite response error from controller and sending 403 Forbidden
*/
bool SofiaSiphandler::handleInviteRespErrFromCtrl(xGateSofiaobj *sobj, xGateCallObj *callObj)
{
	if (!sobj || !callObj)
	{
		XGLOG_ERROR("SofiaSiphandler::handleInviteRespErrFromCtrl sobj or callObj nullptr");
		return false;
	}

	nua_respond((nua_handle_t *)sobj->sofianh, 403, "Forbidden ", TAG_END());
	XGLOG_ERROR("SofiaSiphandler::handleInviteRespErrFromCtrl sending 403 Forbidden to client");
	return true;
}

/*
 * handle inbound pstn invite response error from controller and 403 Forbidden 
 */
bool SofiaSiphandler::handleInviteInboundPstnRespErrFromCtrl(xGateSofiaobj *sobj, xGateCallObj *callObj)
{
	if (!sobj || !callObj)
	{
		XGLOG_ERROR("SofiaSiphandler::handleInviteInboundPstnRespErrFromCtrl sobj or callObj nullptr");
		return false;
	}

    nua_respond((nua_handle_t *)sobj->sofianh, 403, "Forbidden", TAG_END());
	XGLOG_ERROR("SofiaSiphandler::handleInviteInboundPstnRespErrFromCtrl sending 403 Forbidden to client callid(%s)",sobj->callid.c_str());
	return true;

}

bool SofiaSiphandler::handleOptionsRequestFromMonitorService(xGateCallObj *callObj)
{
	if (!callObj)
	{
		XGLOG_ERROR("SofiaSiphandler::handleOptionsRequestFromMonitorService  callObj nullptr");
		return false;
	}

	XGLOG_INFO("SofiaSiphandler::handleOptionsRequestFromMonitorService received EN_OPTIONS_REQUEST callid(%s)",callObj->callid.c_str());
	xGateSofiaobj *sobj = new xGateSofiaobj();
	sobj->callid = callObj->callid.c_str();
	sobj->isTrunkCall = true;
	sobj->m_iTrunkId = callObj->m_iTrunkId;
	sobj->m_sRouteIP = callObj->m_sRouteIP.c_str();
	sobj->m_sCallingNumber = callObj->m_sCallingNumber.c_str();
	SofiaSendOptionsRequest(sobj);
	return true;
}

void SofiaSiphandler::ProcessWssInviteRequest(nua_handle_t *nh, sip_t const *sip, tagi_t tags[],
											  std::string callidstring, unsigned int auth_status)
{
	XGLOG_FUNCTION_EN_EX("");
	XGLOG_INFO("SofiaSiphandler::ProcessWssInviteRequest(%s)", callidstring.c_str());

	xGateSofiaobj *sobj = nullptr;
	xGateCallPair *callpair = nullptr;

	callpair = GetConnectedList(callidstring.c_str());
	if (callpair)
	{
		sobj = callpair->Orgcall;
		if (sobj)
		{
			sobj->siptag = (void *)sip;
			/***************************************************
			 * ToDo:: Commented by Harshith for mrmory leak fix
			 * **************************************************
			xGateSofiaobj *peerSofiaObj = new xGateSofiaobj();
			peerSofiaObj->cp = sobj->cp;
			peerSofiaObj->cp.call_id = callidstring.c_str();
			XGLOG_INFO("SofiaSiphandler::ProcessWssInviteRequest(%s)", peerSofiaObj->cp.call_id.c_str());
			peerSofiaObj->uid = sobj->uid.c_str();
			peerSofiaObj->cp.callStream = "OUTBOUND";
			peerSofiaObj->callstream = EN_OUTBOUND;

			peerSofiaObj->cp.m_AddrRecords.m_strProxyusername = sobj->cp.m_AddrRecords.m_strProxyusername;
			peerSofiaObj->cp.m_AddrRecords.m_strContactaddress = sobj->cp.m_AddrRecords.m_strContactaddress;
			peerSofiaObj->cp.m_AddrRecords.m_strIpaddress = sobj->cp.m_AddrRecords.m_strIpaddress;
			peerSofiaObj->cp.m_AddrRecords.m_strDeviceType = sobj->cp.m_AddrRecords.m_strDeviceType;

			peerSofiaObj->SofiaLegB = sobj;
			peerSofiaObj->replaceinfo = sobj->replaceinfo;
			**************************/

			if (auth_status == 1)
			{
				sobj->callauthstate = 1;
			}

			if (sobj->callauthstate == 1 && sobj->mediastate == EN_MED_SET)
			{

				if (PrepareHandleAndSdpMakeCall(sobj, sobj->cp, sobj->SofiaLegB) == FAIL)
				{
					XGLOG_ERROR("Dail Out failed due to callstate or media state");
				}
				else
				{
					XGLOG_WARN("Dail Out is Success");
				}
			}
			else
				XGLOG_WARN("Dail Out failed due to callstate or media state");
		}
		else
		{
			XGLOG_WARN("callpair object is null");
		}
	}
}

void SofiaSiphandler::ProcessOptionsRequest(xGateSofiaobj *sobj, sip_t const *sip, tagi_t tags[], std::string callidstring)
{
	XGLOG_FUNCTION_EN_EX("");
	if (!sobj)
	{
		XGLOG_INFO("SofiaSiphandler::ProcessOptionsRequest sobj is nullptr");
		return;
	}

	XGLOG_INFO("SofiaSiphandler::ProcessOptionsRequest(%s)", callidstring.c_str());

	xGateSofiaobj *dialsobj = new xGateSofiaobj();
	dialsobj->cp = sobj->cp;
	dialsobj->cp.call_id = callidstring.c_str();
	dialsobj->SofiaLegB = sobj;

	nua_handle_t *handle = nullptr;
	string sTransProto("");
	string sRouteIP("");
	string sContact("");
	handle = CreateSofiaDialhandle(dialsobj->cp, (sip_t *)sobj->siptag, sTransProto, sRouteIP,sContact);

	if (handle != nullptr)
	{
		sobj->SofiaLegB->sofianh = (void *)handle;
		SofiaBindhandle(handle, (nua_hmagic_t *)sobj->SofiaLegB);
	}

	nua_options(handle, SIPTAG_SUPPORTED_STR(""), TAG_END());
}

bool SofiaSiphandler::VerifyForFraudCall(std::string host)
{

	std::vector<std::string> t_wlist_domains = CONFIGSERVICE->m_lightWeightWSSProfile.m_whitelistDomainList;
	auto trunk_map = CONFIGSERVICE->m_lightWeightWSSProfile.m_TrunkMap;

	if (t_wlist_domains.empty())
	{
		XGLOG_ERROR("SofiaSiphandler::VerifyForFraudCall domain whitelist is empty");
		return false;
	}

	std::vector<std::string>::iterator it;
	it = std::find(t_wlist_domains.begin(), t_wlist_domains.end(), host.c_str());
	if (it != t_wlist_domains.end())
	{
		XGLOG_INFO("SofiaSiphandler::VerifyForFraudCall Domain found in whitelist");
		return true;
	}

	std::vector<std::string>::iterator itt;
	for (itt = t_wlist_domains.begin(); itt != t_wlist_domains.end(); itt++)
	{
		std::string tempstr = *itt;
		if ((WildcardPatternMatching(host, tempstr, strlen(host.c_str()), strlen(tempstr.c_str())) == true))
			return true;
	}

	// Cheking whether the host is whitelisted in trunk map or not
	if (trunk_map.find(host.c_str()) != trunk_map.end())
	{
		return true;
	}

#if 0 // Siva::commented below function call because below function not doing proper pattern matching
	if (true == validateDomainWithWildcardWhitelist(host)) {
		XGLOG_INFO("SofiaSiphandler::VerifyForFraudCall Matching entrty found in Wildcard whitelist");
		return true;
	}
#endif

	XGLOG_ERROR("SofiaSiphandler::VerifyForFraudCall (%s) not found in domain whitelist", host.c_str());
	return false;
}

void SofiaSiphandler::RedirectOnReceiving302(xGateSofiaobj *sobj)
{

	XGLOG_FUNCTION_EN_EX("");

	if (sobj)
	{
		xGateSofiaobj *peerSofiaObj = new xGateSofiaobj();
		peerSofiaObj->cp = sobj->cp;
		peerSofiaObj->cp.call_id = sobj->cp.call_id.c_str();
		peerSofiaObj->uid = sobj->uid.c_str();
		peerSofiaObj->cp.callStream = "OUTBOUND";
		peerSofiaObj->callstream = EN_OUTBOUND;

		peerSofiaObj->cp.m_AddrRecords.m_strProxyusername = sobj->cp.m_AddrRecords.m_strProxyusername;
		peerSofiaObj->cp.m_AddrRecords.m_strContactaddress = sobj->cp.m_AddrRecords.m_strContactaddress;
		peerSofiaObj->cp.m_AddrRecords.m_strIpaddress = sobj->cp.m_AddrRecords.m_strIpaddress;
		peerSofiaObj->cp.m_AddrRecords.m_strDeviceType = sobj->cp.m_AddrRecords.m_strDeviceType;

		// peerSofiaObj->SofiaLegB = sobj->SofiaLegB;
		peerSofiaObj->SofiaLegB = sobj;
		peerSofiaObj->replaceinfo = sobj->replaceinfo;

		if (PrepareHandleAndSdpMakeCall(sobj, sobj->cp, peerSofiaObj) == FAIL)
		{
			XGLOG_ERROR("SofiaSiphandler::RedirectOnReceiving302 Dail Out failed");
		}
		else
		{
			XGLOG_WARN("SofiaSiphandler::RedirectOnReceiving302 Dail Out is Success");
		}
	}
	else
	{
		XGLOG_WARN("SofiaSiphandler::RedirectOnReceiving302 callpair object is null");
	}
}

/*
* Check the Domain is Microsoft Teams call or not
*/
bool SofiaSiphandler::isMSTeamsCall(std::string host)
{
	XGLOG_FUNCTION_EN_EX("");
	auto t_ms_team_domain_list = CONFIGSERVICE->m_lightWeightWSSProfile.m_msTeamsDomainList;
	if(t_ms_team_domain_list.empty())
	{
		XGLOG_ERROR("SofiaSiphandler::isMSTeamsCall domain list is empty !");
		return false;
	}

	auto it = std::find(t_ms_team_domain_list.begin(), t_ms_team_domain_list.end(), host.c_str());
	if (it != t_ms_team_domain_list.end())
	{
		XGLOG_INFO("SofiaSiphandler::isMSTeamsCall Domain found in list");
		return true;
	}

	for (auto itt = t_ms_team_domain_list.begin(); itt != t_ms_team_domain_list.end(); itt++)
	{
		std::string tempstr = *itt;
		if ((WildcardPatternMatching(host, tempstr, strlen(host.c_str()), strlen(tempstr.c_str())) == true))
			return true;
	}
	return false;
}


// TODO::Siva //below funtion need to rewrite properly
bool SofiaSiphandler::validateDomainWithWildcardWhitelist(std::string recvd_addr)
{
	XGLOG_FUNCTION_EN_EX("");
	std::vector<std::string> t_wlist_domains = CONFIGSERVICE->m_lightWeightWSSProfile.m_whitelistDomainList;

	if (t_wlist_domains.empty())
	{
		XGLOG_ERROR("SofiaSiphandler::validateDomainWithWildcardWhitelist domain whitelist is empty");
		return false;
	}

	std::string wildcard_delim("*");
	bool t_addr_matched(false);

	std::vector<std::string>::iterator it;
	for (it = t_wlist_domains.begin(); it != t_wlist_domains.end(); it++)
	{
		std::string config_addr = *it;
		stringstream check1(config_addr);
		stringstream check2(recvd_addr);

		std::string intermediate1;
		std::string intermediate2;

		bool t_token_matched(false);
		while ((getline(check1, intermediate1, '.')) && (getline(check2, intermediate2, '.')))
		{
			if (intermediate1 != intermediate2)
			{
				if (intermediate1 != wildcard_delim)
				{
					t_token_matched = false;
					break;
				}
				else
				{
					t_token_matched = true;
				}
			}
			else
			{
				t_token_matched = true;
			}
		}
		if (true == t_token_matched)
		{
			t_addr_matched = true;
			break;
		}
	}

	if (true == t_addr_matched)
	{
		XGLOG_INFO("SofiaSiphandler::validateDomainWithWildcardWhitelist Matching wildcard entry found");
		return true;
	}

	XGLOG_INFO("SofiaSiphandler::validateDomainWithWildcardWhitelist NO Matching Entry Found");
	return false;
}

bool SofiaSiphandler::validateDomainWithWildcardRouteAddr(std::string recvd_addr, std::string conf_roomId_Route, std::string *bridge_ip)
{
	XGLOG_FUNCTION_EN_EX("");
	std::map<std::string, BridgeRouteIpList *> t_inboundRouteAddrMap = CONFIGSERVICE->m_lightWeightWSSProfile.m_inboundRouteAddrMap;

	BridgeRouteIpList *ptrBridgeIpList = nullptr;

	if (t_inboundRouteAddrMap.empty())
	{
		XGLOG_ERROR("SofiaSiphandler::validateDomainWithWildcardRouteAddr inboundRouteAddrMap is empty");
		return false;
	}

	std::string wildcard_delim("*");
	bool t_addr_matched(false);

	std::map<std::string, BridgeRouteIpList *>::iterator it;
	std::string config_addr = "";

	for (it = t_inboundRouteAddrMap.begin(); it != t_inboundRouteAddrMap.end(); it++)
	{
		config_addr = it->first;

		stringstream check1(config_addr);
		stringstream check2(recvd_addr);

		std::string intermediate1;
		std::string intermediate2;

		bool t_token_matched(false);
		while ((getline(check1, intermediate1, '.')) && (getline(check2, intermediate2, '.')))
		{
			if (intermediate1 != intermediate2)
			{
				if (intermediate1 != wildcard_delim)
				{
					t_token_matched = false;
					break;
				}
				else
				{
					t_token_matched = true;
				}
			}
			else
			{
				t_token_matched = true;
			}
		}
		if (true == t_token_matched)
		{
			t_addr_matched = true;
			ptrBridgeIpList = it->second;
			//  *bridge_ip = ptrBridgeIpList->getBridgeIpToRoute();
			break;
		}
	}

	if (true == t_addr_matched)
	{
		XGLOG_INFO("SofiaSiphandler::validateDomainWithWildcardWhitelist Matching wildcard entry found");
		if (!conf_roomId_Route.empty())
		{
			XGLOG_INFO("SofiaSiphandler::validateDomainWithWildcardWhitelist get IP from RoomID-Route table");
			*bridge_ip = getRouteIPAddrForDomain(config_addr, conf_roomId_Route);
		}
		else
		{
			XGLOG_INFO("SofiaSiphandler::validateDomainWithWildcardWhitelist get IP from Round Robin table");
			*bridge_ip = ptrBridgeIpList->getBridgeIpToRoute();
		}

		return true;
	}

	XGLOG_ERROR("No atching address found");
	return false;
}

bool SofiaSiphandler::validateDomainWithWildcardRouteAddrSerialRouting(std::string recvd_addr, std::string *bridge_ip,
																	   bool isReRoute, std::string sRoutedIP)
{
	XGLOG_FUNCTION_EN_EX("");
	std::map<std::string, BridgeRouteIpList *> t_inboundRouteAddrMap = CONFIGSERVICE->m_lightWeightWSSProfile.m_inboundRouteAddrMap;

	BridgeRouteIpList *ptrBridgeIpList = nullptr;

	if (t_inboundRouteAddrMap.empty())
	{
		XGLOG_ERROR("SofiaSiphandler::validateDomainWithWildcardRouteAddr inboundRouteAddrMap is empty");
		return false;
	}

	std::string wildcard_delim("*");
	bool t_addr_matched(false);

	std::map<std::string, BridgeRouteIpList *>::iterator it;
	std::string config_addr = "";

	for (it = t_inboundRouteAddrMap.begin(); it != t_inboundRouteAddrMap.end(); it++)
	{
		config_addr = it->first;

		stringstream check1(config_addr);
		stringstream check2(recvd_addr);

		std::string intermediate1;
		std::string intermediate2;

		bool t_token_matched(false);
		while ((getline(check1, intermediate1, '.')) && (getline(check2, intermediate2, '.')))
		{
			if (intermediate1 != intermediate2)
			{
				if (intermediate1 != wildcard_delim)
				{
					t_token_matched = false;
					break;
				}
				else
				{
					t_token_matched = true;
				}
			}
			else
			{
				t_token_matched = true;
			}
		}
		if (true == t_token_matched)
		{
			t_addr_matched = true;
			ptrBridgeIpList = it->second;
			if (!isReRoute)
			{
				*bridge_ip = ptrBridgeIpList->getBridgeIpToRoute();
			}
			else
			{
#if 1
				// int size = sizeof(ptrBridgeIpList->m_addrList)/sizeof(ptrBridgeIpList->m_addrList[0]);
				for (int i = 0; i < 4; i++)
				{
					if (ptrBridgeIpList->m_addrList[i] != sRoutedIP)
					{
						*bridge_ip = ptrBridgeIpList->m_addrList[i].c_str();
						break;
					}
				}
#endif
			}

			break;
		}
	}

	if (true == t_addr_matched)
	{
		XGLOG_INFO("SofiaSiphandler::validateDomainWithWildcardWhitelist Matching wildcard entry found");
		return true;
	}

	XGLOG_ERROR("No atching address found");
	return false;
}

std::string SofiaSiphandler::getRouteIPAddrForDomain(std::string domain_name, std::string conf_room_id)
{
	XGLOG_FUNCTION_EN_EX("");
	if (domain_name.empty() || conf_room_id.empty())
	{
		XGLOG_ERROR("Domain name and Room ID are empty");
		return std::string();
	}

	XGLOG_INFO("SofiaSiphandler::getRouteIPAddrForDomain Received Domain(%s) and RoomID(%s)", domain_name.c_str(), conf_room_id.c_str());
	std::string route_ip = "";

	std::map<std::string, std::list<std::pair<std::string, std::string>>>::iterator itr;
	itr = m_domainRouteMap.find(domain_name);

	if (itr == m_domainRouteMap.end())
	{
		/*domain is not present*/
		XGLOG_INFO("SofiaSiphandler::getRouteIPAddrForDomain No Entry found for the domain(%s)", domain_name.c_str());

		std::pair<std::string, std::string> pair_roomIDrouteIP;
		pair_roomIDrouteIP.first = conf_room_id;
		route_ip = pair_roomIDrouteIP.second = CONFIGSERVICE->m_lightWeightWSSProfile.getInboundRouteIp(domain_name);

		std::list<std::pair<std::string, std::string>> roomIDList;
		roomIDList.push_back(pair_roomIDrouteIP);

		m_domainRouteMap[domain_name] = roomIDList;
	}
	else
	{
		/*domain is already present*/
		XGLOG_INFO("SofiaSiphandler::getRouteIPAddrForDomain:  Entry found for the domain(%s)", domain_name.c_str());

		std::list<std::pair<std::string, std::string>> t_roomIDList;
		t_roomIDList = itr->second;

		std::list<std::pair<std::string, std::string>>::iterator itrl;

		bool room_found_flag = false;
		for (itrl = t_roomIDList.begin(); itrl != t_roomIDList.end(); itrl++)
		{
			std::pair<std::string, std::string> t_roomIDrouteIP;
			if (itrl->first == conf_room_id)
			{
				XGLOG_INFO("SofiaSiphandler::getRouteIPAddrForDomain: Found route IP Pair found for the Room ID:(%s)", conf_room_id.c_str());
				route_ip = itrl->second;
				room_found_flag = true;
				break;
			}
		}

		if (room_found_flag == false)
		{
			/*room not found*/
			XGLOG_INFO("SofiaSiphandler::getRouteIPAddrForDomain: No Route IP Pair found for the Room ID(%s)", conf_room_id.c_str());
			std::pair<std::string, std::string> pair_roomIDrouteIP;
			pair_roomIDrouteIP.first = conf_room_id;
			route_ip = pair_roomIDrouteIP.second = CONFIGSERVICE->m_lightWeightWSSProfile.getInboundRouteIp(domain_name);
			t_roomIDList.push_back(pair_roomIDrouteIP);
			m_domainRouteMap[domain_name] = t_roomIDList;
		}
	}

	// cout << "Route IP: " << route_ip.c_str() << endl;

	return route_ip;
}

/*Wildcard Pattern Matching
 *DP Initialization Algorithm used
 *matching against with *
 * */
bool SofiaSiphandler::WildcardPatternMatching(std::string recv_addr, std::string pattern, int recv_length, int pattern_length)
{
	if (recv_addr.empty() || pattern.empty())
	{
		XGLOG_ERROR("SofiaSiphandler::WildcardPatternMatching failed due to recv_addr is empty");
		return false;
	}
	if (pattern_length == 0)
		return (recv_length == 0);

	// lookup table for storing results of
	// subproblems
	bool lookup[recv_length + 1][pattern_length + 1];

	// initialize lookup table to false
	memset(lookup, false, sizeof(lookup));

	// empty pattern can match with empty string
	lookup[0][0] = true;

	// Only '*' can match with empty string
	for (int j = 1; j <= pattern_length; j++)
		if (pattern[j - 1] == '*')
			lookup[0][j] = lookup[0][j - 1];

	// fill the table in bottom-up fashion
	for (int i = 1; i <= recv_length; i++)
	{
		for (int j = 1; j <= pattern_length; j++)
		{
			// Two cases if we see a '*'
			// a) We ignore ‘*’ character and move
			// to next character in the pattern,
			//	 i.e., ‘*’ indicates an empty sequence.
			// b) '*' character matches with ith
			//	 character in input
			if (pattern[j - 1] == '*')
				lookup[i][j] = lookup[i][j - 1] || lookup[i - 1][j];

			// Current characters are considered as
			// matching in two cases
			// (a) current character of pattern is '?'
			// (b) characters actually match
			/*else if (pattern[j - 1] == '?'
				|| str[i - 1] == pattern[j - 1])
				lookup[i][j] = lookup[i - 1][j - 1];*/
			else if (recv_addr[i - 1] == pattern[j - 1])
				lookup[i][j] = lookup[i - 1][j - 1];

			// If characters don't match
			else
				lookup[i][j] = false;
		}
	}

	return lookup[recv_length][pattern_length];
}

void SofiaSiphandler::ReRouteCallToOtherNode(xGateSofiaobj *sobj)
{
	XGLOG_FUNCTION_EN_EX("");
	std::string sRouteIP("");

	if (sobj)
	{
		sobj->isCallReRouted = true;
		xGateSofiaobj *peerSofiaObj = new xGateSofiaobj();
		peerSofiaObj->cp = sobj->cp;
		peerSofiaObj->cp.call_id = sobj->cp.call_id.c_str();
		peerSofiaObj->uid = sobj->uid.c_str();
		peerSofiaObj->cp.callStream = "OUTBOUND";
		peerSofiaObj->callstream = EN_OUTBOUND;

		peerSofiaObj->cp.m_AddrRecords.m_strProxyusername = sobj->cp.m_AddrRecords.m_strProxyusername;
		peerSofiaObj->cp.m_AddrRecords.m_strContactaddress = sobj->cp.m_AddrRecords.m_strContactaddress;
		peerSofiaObj->cp.m_AddrRecords.m_strIpaddress = sobj->cp.m_AddrRecords.m_strIpaddress;
		peerSofiaObj->cp.m_AddrRecords.m_strDeviceType = sobj->cp.m_AddrRecords.m_strDeviceType;

		// peerSofiaObj->SofiaLegB = sobj->SofiaLegB;
		peerSofiaObj->SofiaLegB = sobj;
		peerSofiaObj->replaceinfo = sobj->replaceinfo;

		FetchNextRoute(sobj, sRouteIP);

		if (!sRouteIP.empty())
			sobj->cp.forwardingmgcrouteIp = sRouteIP.c_str();
		else
			return;

		if (PrepareHandleAndSdpMakeCall(sobj, sobj->cp, peerSofiaObj) == FAIL)
		{
			XGLOG_ERROR("SofiaSiphandler::ReRouteCallToOtherNode Dail Out failed uid:%s", sobj->uid.c_str());
		}
		else
		{
			XGLOG_WARN("SofiaSiphandler::ReRouteCallToOtherNode Dail Out is Success uid:%s", sobj->uid.c_str());
			sobj->isCallReRouted = true;
		}
	}
	else
	{
		XGLOG_WARN("SofiaSiphandler::RedirectOnReceiving302 callpair object is null");
	}
}

#if 1
bool SofiaSiphandler::FetchNextRoute(xGateSofiaobj *sobj, std::string &sRouteIP)
{
	XGLOG_FUNCTION_EN_EX("");
	if (!sobj)
		XGLOG_ERROR("SofiaSiphandler::FetchNextRoute sobj is nullptr!");

	std::string sDomainName(""), sRouteAddress("");

	if ((sip_t *)sobj->siptag && ((sip_t *)sobj->siptag)->sip_to && ((sip_t *)sobj->siptag)->sip_to->a_url)
		sDomainName = ((sip_t *)sobj->siptag)->sip_to->a_url->url_host;
	else
		return false;

	if (validateDomainWithWildcardRouteAddrSerialRouting(sDomainName, &sRouteAddress, true, sobj->m_sRouteIP.c_str()) == true)
	{
		if (!sRouteAddress.empty())
			sRouteIP = sRouteAddress;
	}

	return true;
}
#endif

xGateReturn SofiaSiphandler::ForwardReferRequest(xGateSofiaobj *sofiaobj, int status, char const *phrase, nua_t *nua,
												 nua_magic_t *magic, nua_handle_t *nh, nua_hmagic_t *hmagic, sip_t const *sip,
												 tagi_t tags[])
{
	XGLOG_FUNCTION_EN_EX("");
	XGLOG_INFO("SofiaSiphandler::ForwardReferRequest(%s) Request", sofiaobj->callid.c_str());
	nua_refer((nua_handle_t *)sofiaobj->SofiaLegB->sofianh, SIPTAG_REFER_TO(sip->sip_refer_to),
			  SIPTAG_REFERRED_BY(sip->sip_referred_by), NUTAG_AUTOACK(1), TAG_END());

	XGLOG_INFO("SofiaSiphandler::ForwardReferRequest Sent out....");
	return SUCCESS;
}

void SofiaSiphandler::OnReferRequestAttendent(xGateSofiaobj *sobj, int status, char const *phrase, nua_t *nua,
											  nua_magic_t *magic, nua_handle_t *nh, nua_hmagic_t *hmagic, sip_t const *sip,
											  tagi_t tags[])
{

	XGLOG_FUNCTION_EN_EX("");
	// Added for Attend Transfer -starts
	char const *params = (char const *)sip->sip_refer_to->r_url->url_headers;
	XGLOG_INFO("Replace header contanis =(%s)", params);

	SofiaSiphandlerReplaceHeaderParser replaceHeaderparse;
	replaceHeaderparse.parseReplaceHeader(params);

	std::string t_extractedcallid("");
	std::string t_replaceHdrCallId = replaceHeaderparse.replaceHdrCallID;
	std::string t_referredByExt("");
	if (sip && sip->sip_referred_by && sip->sip_referred_by->b_url)
	{
		t_referredByExt = sip->sip_referred_by->b_url->url_user;
		if (!t_referredByExt.empty())
		{
			t_extractedcallid = t_replaceHdrCallId.c_str();
		}
	}
	XGLOG_INFO("SofiaSiphandler::OnReferRequestAttendent t_extractedcallid = (%s)", t_extractedcallid.c_str());

	xGateSofiaobj *sofiatempobj = nullptr;
	xGateCallPair *callpair = nullptr;

	callpair = GetConnectedList(t_extractedcallid.c_str());
	if (callpair == nullptr)
	{
		// In deskphone we will not get actual call id in replace header so we need to append from replaceHdrToTag
		t_extractedcallid.append("@");
		t_extractedcallid.append(replaceHeaderparse.replaceHdrToTag);
		callpair = GetConnectedList(t_extractedcallid.c_str());
	}
	if (callpair && callpair->Orgcall)
	{
		sofiatempobj = callpair->Orgcall;
	}
	else
	{
		XGLOG_INFO("SofiaSiphandler::OnReferRequestAttendent sofiatempobj is not found");
		return;
	}
	// std::string t_replacesstring = std::string("Replaces=") + sofiatempobj->SofiaLegB->replaceinfo.Responsecallid + std::string("%3Bto-tag%3D") + sofiatempobj->SofiaLegB->replaceinfo.to_tag + std::string("%3Bfrom-tag%3D") + sofiatempobj->SofiaLegB->replaceinfo.from_tag;
	std::string t_replacesstring = std::string("Replaces=") + sofiatempobj->SofiaLegB->replaceinfo.Invitecallid + std::string("%3Bto-tag%3D") + sofiatempobj->SofiaLegB->replaceinfo.to_tag + std::string("%3Bfrom-tag%3D") + sofiatempobj->SofiaLegB->replaceinfo.from_tag;

	sip->sip_refer_to->r_url->url_headers = strdup(t_replacesstring.c_str());

	char const *printparams = (char const *)sip->sip_refer_to->r_url->url_headers;
	XGLOG_INFO("SofiaSiphandler::OnReferRequestAttendent printparams = (%s)", printparams);
	cout << printparams << endl;

	// Added for Attend Transfer -Ends

	xGateSofiaobj *sofiaobj = (xGateSofiaobj *)hmagic;
	XGLOG_INFO("SofiaSihandler::OnReferRequest Call Transfer call uid %s", sofiaobj->callid.c_str());
	nua_refer((nua_handle_t *)sofiaobj->SofiaLegB->sofianh, SIPTAG_REFER_TO(sip->sip_refer_to), SIPTAG_REFERRED_BY(sip->sip_referred_by), TAG_END());
}

bool SofiaSiphandler::ValidateSipMessage(sip_t const *sip, SIPVALIDATE header)
{
	if (sip)
	{
		switch (header)
		{
		case SIPVALIDATE::FROM:

			if (!sip->sip_from ||
				!sip->sip_from->a_url->url_user || !sip->sip_from->a_url->url_host ||
				!sip->sip_from->a_tag)
			{
				XGLOG_WARN("SofiaSiphandler::ValidateSipMessage", "Failed-Sip From Header Validation");
				return false;
			}
			break;
		case SIPVALIDATE::REQUEST:
			if (!sip->sip_request ||
				!sip->sip_request->rq_url->url_user || !sip->sip_request->rq_url->url_host ||
				!sip->sip_request->rq_url->url_port)
			{
				XGLOG_WARN("SofiaSiphandler::ValidateSipMessage", "Failed-Sip Request Header Validation");
				return false;
			}
			break;
		case SIPVALIDATE::CONTACT:
			if (!sip->sip_contact)
			{
				XGLOG_WARN("SofiaSiphandler::ValidateSipMessage", "Failed-Sip Contact Header Validation");
				return false;
			}
			break;
		case SIPVALIDATE::TO:
			if (!sip->sip_to ||
				!sip->sip_to->a_url->url_user || !sip->sip_to->a_url->url_host)
			{
				XGLOG_WARN("SofiaSiphandler::ValidateSipMessage", "Failed-Sip To Header Validation");
				return false;
			}
			break;
		case SIPVALIDATE::TAG:
			break;
		case SIPVALIDATE::CALLID:
			if (!sip->sip_call_id ||
				!sip->sip_call_id->i_id)
			{
				XGLOG_WARN("SofiaSiphandler::ValidateSipMessage", "Failed-Sip Callid Header Validation");
				return false;
			}
			break;
		case SIPVALIDATE::USER_AGENT:
			if (!sip->sip_user_agent ||
				!sip->sip_user_agent->g_string)
			{
				XGLOG_WARN("SofiaSiphandler::ValidateSipMessage", "Failed-Sip User Agent Header Validation");
				return false;
			}
			break;
		case SIPVALIDATE::VIA:
			if(!sip->sip_via)
			{
				XGLOG_ERROR("SofiaSiphandler::ValidateSipMessage", "Failed-Sip Via Header Validation");
				return false;
			}
			break;
		default:
			XGLOG_WARN("SofiaSiphandler::ValidateSipMessage(%s)", "No Header option to check");
			return false;
			break;
		} // swicth case Header
	}	  // Sip check point
	else
	{
		XGLOG_WARN("SofiaSiphandler::ValidateSipMessage(%s)", "Failed-Sip Structure Validation");
		return false;
	}
	return true;

} // ValidateSipMessage

/*
* Encoding Sip Header string for Dialout 
*/
std::string SofiaSiphandler::EncodeSipHeader(url_t *url, callInfoParameter cp, SIPVALIDATE header,std::string sIPAddress,std::string sTransport,bool isServerCall,sip_p_asserted_identity_t *paidurl)
{
	std::stringstream sipstr;
	std::string sSchema{"sip"};
	std::string sPort{m_pSipHndlrParams->m_sSipPort};
	if (sTransport.compare("tls") == 0)
	{
		sSchema = "sips";
		sPort = m_pSipHndlrParams->m_sSipsPort.c_str();
	}
	else if (sTransport.compare("wss") == 0)
	{
		sPort = m_pSipHndlrParams->m_sWssPort.c_str();
	}

	switch (header)
	{
	case FROM:
	{
		// 530 <sip:530@46.43.144.82;transport=wss>
		sipstr << url->url_user << "<" << sSchema << ":" << url->url_user << "@" << ((!isServerCall)? url->url_host : sIPAddress) << ";transport=" << sTransport << ">";
	}
	break;
	case TO:
	{
		// 664 <sip:664@182.66.104.114:49936;transport=wss>
		sipstr << url->url_user << "<" << sSchema << ":" << url->url_user << "@" << url->url_host << ":" << ((isServerCall)? url->url_port : sPort) << ";transport=" << sTransport << ">";
	}
	break;
	case CONTACT:
	{
		// 530 <sip:530@46.43.144.82:57344;transport=wss>
		sipstr << url->url_user << "<" << sSchema << ":" << url->url_user << "@" << sIPAddress << ":" << sPort << ";transport=" << sTransport << ">";
	}
	break;
	case ROUTE:
	{
		//<sip:46.43.144.82:5060;lr>
   		sipstr << "<sip:" << sIPAddress << ":" <<  m_pSipHndlrParams->m_sSipPort << ";lr>";
	}
	break;
	case PROXY_ROUTE:
	{
		// sip:182.66.104.114:49936;transport=wss
		sipstr << sSchema << ":" << ((isServerCall) ? url->url_host : sIPAddress) << ":"
			   << ((isServerCall) ? url->url_port : sPort)
			   << ";transport=" << sTransport;
	}
	break;
	case PAI:
	{
		//918754784191 <sip:918754784191@10.22.3.228:5060;user=phone>
		sipstr << paidurl->paid_url->url_user << " <sip:" << paidurl->paid_url->url_user << "@" << sIPAddress << ":" << sPort<< ";user=phone>";
	}
	break;
	default:
	{
		XGLOG_ERROR("SofiaSiphandler::EncodeSipHeader unknown Header found !");
	}
	}
	return sipstr.str();
}

bool SofiaSiphandler::VerifyAuthorisationResponce(sip_t const *sip, std::string password, std::string ReqMethod)
{
	auth_response_t ar[1] = {{0}};
	char const *md5 = nullptr, *md5sess = nullptr, *sha1 = nullptr,
			   *qop_auth = nullptr, *qop_auth_int = nullptr;
	struct msg_auth_s *rr = nullptr;

	if(ReqMethod.compare("INVITE")==0)
	{
		rr = sip->sip_proxy_authorization;
	}
	else
	{
		rr = sip->sip_authorization;
	}
	if (rr)
	{
		auth_get_params(&m_homeMemory, rr->au_params,
						"username=", &ar->ar_username,
						"realm=", &ar->ar_realm,
						"nonce=", &ar->ar_nonce,
						"uri=", &ar->ar_uri,
						"response=", &ar->ar_response,
						"algorithm=", &ar->ar_algorithm,
						"opaque=", &ar->ar_opaque,
						"cnonce=", &ar->ar_cnonce,
						"qop=", &ar->ar_qop,
						"nc=", &ar->ar_nc,
						"algorithm=md5", &md5,
						"algorithm=md5-sess", &md5sess,
						"algorithm=sha1", &sha1,
						"qop=auth", &qop_auth,
						"qop=auth-int", &qop_auth_int,
						nullptr);
		auth_hexmd5_t ha1;
		auth_digest_a1(ar, ha1, password.c_str());

		// Creating  HA2 and responce
		auth_hexmd5_t Regresponce;
		auth_digest_response(ar, Regresponce, ha1, ReqMethod.c_str(), nullptr, 0);
		XGLOG_INFO("\n**SofiaSiphandler VerifyAuthorisationResponceUsername = (%s)&& password = (%s)",
				   ar->ar_username, password.c_str());
		XGLOG_INFO("SofiaSiphandler VerifyAuthorisationResponce Create Hash %s -- Received Hash %s ",
					 Regresponce, ar->ar_response);
		if (strstr(ar->ar_response, Regresponce) != nullptr)
		{
			XGLOG_INFO("SofiaSiphandler VerifyAuthorisationResponce Create Hash %s -- Received Hash %s both are same",
						 Regresponce, ar->ar_response);
			return true;
		}
	}
	return false;
}

bool SofiaSiphandler::AuthenticateSipSession(int status, char const *phrase, nua_t *nua,
											 nua_magic_t *magic, nua_handle_t *nh, nua_hmagic_t *hmagic, sip_t const *sip,
											 tagi_t tags[])
{

	XGLOG_FUNCTION_EN_EX("");

	xGateSofiaSipRegobj *sipregobj = new xGateSofiaSipRegobj();
	CreateSofiaRegobj(*sipregobj, sip);
	XGLOG_INFO("AuthenticateSipSession(%s)", sipregobj->callid.c_str());
	sipregobj->Reghandle = (void *)nh;
	sipregobj->sofiamh = (void *)magic;
	sipregobj->siptags = (void *)tags;
	bool ret = false;

	if (sip->sip_proxy_authorization)
	{ // Request has auth paramaters

		// Post message to DB thread
		if (sip && sip->sip_request && sip->sip_request->rq_url && sip->sip_request->rq_url->url_host)
		{
			if (CheckDomainForCallAuth(sip->sip_request->rq_url->url_host) == false)
				if (sip && sip->sip_from && sip->sip_from->a_url && sip->sip_from->a_url->url_host)
				{
					sipregobj->domainname = sip->sip_from->a_url->url_host;
				}
		}
		sipregobj->Regevent = EN_AUTHCALL;

		xGateCallObj *callobj = new xGateCallObj;
		callobj->m_regObj = sipregobj;
		callobj->sipeventtype = EN_REGISTRATION_REQ;
		PostMessage(callobj, XGATEPOSTTYPE::EN_PROCESS);

		XGLOG_INFO("AuthenticateSipSession(%s) - Verification Request to DB Manager", sipregobj->callid.c_str());
		ret = true;
	}
	else
	{
		// Send 407

		XGLOG_INFO("AuthenticateSipSession(%s) - Sending 407 proxy authentication required details back", sipregobj->callid.c_str());
		sipregobj->Regevent = EN_SEND407;
		RegistarResponse(sipregobj);
	}
	return ret;
}

/*
 *Setting Proxy URL if mismatch happening in conatct and via
 */
void SofiaSiphandler::SofiaSetProxyRoute(nua_handle_t *nh, sip_t const *sip, string callid)
{

	std::string url(""), sTransPort("");
	if (sip && sip->sip_via)
	{
		if (strstr(sip->sip_via->v_protocol, "SIP/2.0/TLS"))
			sTransPort = "tls";
		else if (strstr(sip->sip_via->v_protocol, "SIP/2.0/WSS"))
			sTransPort = "wss";
		else if(strstr(sip->sip_via->v_protocol, "SIP/2.0/TCP"))
			sTransPort = "tcp";
		else 
			sTransPort = "udp";
	}

	if (sip->sip_contact && sip->sip_contact->m_url && sip->sip_contact->m_url->url_port)
	{
		if (sip && sip->sip_via && sip->sip_via->v_received && sip->sip_via->v_rport)
		{
			if (strcmp(sip->sip_contact->m_url->url_host, sip->sip_via->v_received) != 0)
			{
				prepare_url_string(GetValidIP(sip->sip_via->v_received), sip->sip_via->v_rport, sTransPort.c_str(), url);
				nua_set_hparams(nh, NUTAG_PROXY((url_string_t *)url.c_str()), TAG_END());
				XGLOG_ERROR("SofiaSiphandler::SofiaSetProxyRoute callid(%s)IP mismatch so setting new URL:%s", callid.c_str(), url.c_str());
			}
			else if (strcmp(sip->sip_contact->m_url->url_port, sip->sip_via->v_rport) != 0)
			{
				prepare_url_string(GetValidIP(sip->sip_via->v_received), sip->sip_via->v_rport, sTransPort.c_str(), url);
				nua_set_hparams(nh, NUTAG_PROXY((url_string_t *)url.c_str()), TAG_END());
				XGLOG_ERROR("SofiaSiphandler::SofiaSetProxyRoute callid(%s)Port mismatch so setting new URL:%s", callid.c_str(), url.c_str());
			}
		}
		else
		{
			if (sip->sip_via && sip->sip_via->v_rport && strcmp(sip->sip_contact->m_url->url_port, sip->sip_via->v_rport) != 0)
			{
				prepare_url_string(sip->sip_contact->m_url->url_host, sip->sip_via->v_rport, sTransPort.c_str(), url);
				nua_set_hparams(nh, NUTAG_PROXY((url_string_t *)url.c_str()), TAG_END());
				XGLOG_ERROR("SofiaSiphandler::SofiaSetProxyRoute callid(%s)Port mismatch so setting new URL:%s", callid.c_str(), url.c_str());
			}
		}
	}
	else
	{
		prepare_url_string(GetValidIP(sip->sip_via->v_received), sip->sip_via->v_rport, sTransPort.c_str(), url);
		nua_set_hparams(nh, NUTAG_PROXY((url_string_t *)url.c_str()), TAG_END());
		XGLOG_ERROR("SofiaSiphandler::SofiaSetProxyRoute callid(%s)Contact not valid so setting new URL:%s", callid.c_str(), url.c_str());
	}
}

bool SofiaSiphandler::DeleteSofiaObject(xGateSofiaobj *sobj)
{
	XGLOG_FUNCTION_EN_EX("");
	if (sobj)
	{
		if ((sobj)->sofianh)
		{
			SofiaBindhandle((nua_handle_t *)(sobj)->sofianh, nullptr);
			SofiaHandleDestroy((nua_handle_t *)(sobj)->sofianh);
		} // Leg A handle clean up This stops the even for Leg A
		if ((sobj)->SofiaLegB)
		{
			if ((sobj)->SofiaLegB->sofianh && (sobj)->SofiaLegB->isPeerInviteSent)
			{
				SofiaBindhandle((nua_handle_t *)(sobj)->SofiaLegB->sofianh, nullptr);
				SofiaHandleDestroy((nua_handle_t *)(sobj)->sofianh);
			} // Leg B handle clean up this stops the event for Leb B
			// Deleting Leg B First;
			(sobj)->SofiaLegB->DeleteAll();
			delete (sobj)->SofiaLegB;
			(sobj)->SofiaLegB = nullptr;
		}
		// Deleting Leg A Obj
		(sobj)->DeleteAll();
		delete sobj;
		(sobj) = nullptr;
	}
	return true;
}

xGateReturn SofiaSiphandler::PrepareHandleAndSdpMakeCall(xGateSofiaobj *sobj, callInfoParameter cp, xGateSofiaobj *dialsobj)
{
	XGLOG_FUNCTION_EN_EX("");
	XGLOG_INFO("SofiaSiphandler::PrepareHandleAndSdpMakeCall In(%s)", sobj->uid.c_str());
	static int dialout_count;

	XGLOG_INFO("SofiaSiphandler::PrepareHandleAndSdpMakeCall DIALOUT COUNT In(%d)", dialout_count);
	dialout_count++;

	nua_handle_t *handle = nullptr;
	sdp_session_t *psdp = nullptr;
	sdp_session_t *rsdp = nullptr;

	std::string sdpstr("");
	std::string sTransProto("");
	std::string sRouteIP("");
	std::string sContact("");

	//dialsobj->SofiaLegB->callstream = EN_OUTBOUND;
	sobj->SofiaLegB->callstream = EN_OUTBOUND;

	if(!sobj->m_sRouteIP.empty()) {
		sRouteIP = sobj->m_sRouteIP;
	} else {
		sRouteIP = m_pSipHndlrParams->m_sDefaultPBXIP.c_str();
		XGLOG_ERROR("SofiaSiphandler::PrepareHandleAndSdpMakeCall Route IP Empty! for (%s) ,so assigned default IPPBX IP(%s)",\
														sobj->uid.c_str(),m_pSipHndlrParams->m_sDefaultPBXIP.c_str());
	}

	handle = CreateSofiaDialhandle(cp, (sip_t *)sobj->siptag, sTransProto, sRouteIP,sContact,sobj->m_sCallInfo);
	if (handle != nullptr)
	{
		// sobj->SofiaLegB = dialsobj;
		sobj->SofiaLegB->cp.cnCalling = sobj->cp.cnCalling.c_str();
		sobj->SofiaLegB->cp.cnCalled = sobj->cp.cnCalled.c_str();
		sobj->SofiaLegB->sofianh = (void *)handle;
		sobj->SofiaLegB->cp.m_sRespContact = sContact.c_str();
		if(isPBXCall(((sip_t *)sobj->siptag)))
		{
			sobj->SofiaLegB->cp.isClientDialout = true;
		}
#if 0
		if (sobj->siptag && ((sip_t *)sobj->siptag)->sip_via)
		{
			if (strstr(((sip_t *)sobj->siptag)->sip_via->v_protocol, "SIP/2.0/UDP"))
			{
				sobj->SofiaLegB->m_sClientIP = ((sip_t *)sobj->siptag)->sip_to->a_url->url_host;
				if (((sip_t *)sobj->siptag)->sip_to->a_url->url_port)
					sobj->SofiaLegB->m_sClientPort = ((sip_t *)sobj->siptag)->sip_to->a_url->url_port;
				sobj->cp.isCallAnswered = true;

				if ((cp.csTransport.compare("transport=ws;ob") == 0) || (cp.csTransport.compare(";transport=wss") == 0) || (cp.csTransport.compare(";transport=ws") == 0) || (sTransProto.compare("wss") == 0))
				{
					sobj->SofiaLegB->m_sClientTransport = "wss";
				}
				else
				{
					sobj->SofiaLegB->m_sClientTransport = "tls";
				}
			}
			// To set dialout to server transportType as UDP
			else // TODO:Need to set transporttype properly
			{
				sobj->SofiaLegB->cp.csTransport = ";transport=udp";
			}
		}
#endif

		SofiaBindhandle(handle, (nua_hmagic_t *)sobj->SofiaLegB);

		nua_set_hparams((nua_handle_t *)sobj->SofiaLegB->sofianh,
						// SOATAG_AUDIO_AUX("telephone-event"),
						TAG_END());
		nua_set_hparams(handle, NUTAG_ALLOW("UPDATE"), TAG_END());

		nua_invite(handle, NUTAG_AUTOACK(1), NUTAG_INVITE_TIMER(15), SIPTAG_CONTENT_TYPE_STR("application/sdp"),
				   SIPTAG_PAYLOAD_STR(sobj->m_sOffSdp.c_str()), SIPTAG_SUPPORTED_STR(""), TAG_END());
		sobj->isPeerInviteSent = true;
		sobj->SofiaLegB->isPeerInviteSent = true;

 // SIVA::Setting timer for incomming call from IPPBX
		if(isPBXCall((sip_t *)sobj->siptag) && (sTransProto.compare("tls") == 0))
		{
			XGLOG_INFO("SofiaSiphandler::Setting timer for TLS Incomming call from IPPBX requests(%s)", sobj->uid.c_str());
			set_invite_timer(sobj);
		}
	}
	else
	{
		XGLOG_ERROR("SofiaSiphandler::PrepareHandleAndSdpMakeCall(%s) Handle Creation  Failed", sobj->uid.c_str());
		//dialsobj->SofiaLegB->sofianh = nullptr; //Harshith:: commented
		sobj->SofiaLegB->sofianh = nullptr;
		return FAIL;
	}
	if(isPBXCall((sip_t *)sobj->siptag))
		XGLOG_TRACE("Invite_from(%s)_to(%s) domain(%s) callid(%s) Forward to IPPBX(%s)",sobj->m_sCallingNumber.c_str(),\
															  	   						sobj->m_sCalledNumber.c_str(),\
                                                                   						sobj->m_sDomainName.c_str(),\
															       						sobj->callid.c_str(),\
																						sRouteIP.c_str());

	XGLOG_INFO("SofiaSiphandler::PrepareHandleAndSdpMakeCall Out(%s)", sobj->uid.c_str());

	return SUCCESS; 
}

/* ForwardSipInfo handles the forwading the sip info method to peer without processing
 * */
xGateReturn SofiaSiphandler::ForwardSipInfo(xGateSofiaobj *sofiaobj, sip_payload_t *payload)
{
	XGLOG_INFO("SofiaSiphandler::ForwardSipInfo(%s) Request", sofiaobj->uid.c_str());

	nua_info((nua_handle_t *)sofiaobj->SofiaLegB->sofianh, SIPTAG_CONTENT_TYPE_STR(sofiaobj->m_sInfoContentType.c_str()),
			 SIPTAG_PAYLOAD(payload), TAG_END());
	return SUCCESS;
}

/**/
void SofiaSiphandler::ForwardReInvite(xGateSofiaobj *sofiaobj)
{
	XGLOG_FUNCTION_EN_EX("");

	XGLOG_INFO("SofiaSiphandler::ForwardReInvite(%s)", sofiaobj->callid.c_str());
	nua_set_hparams((nua_handle_t *)sofiaobj->SofiaLegB->sofianh, NUTAG_ALLOW("UPDATE"), TAG_END());

	nua_invite((nua_handle_t *)sofiaobj->SofiaLegB->sofianh, NUTAG_AUTOACK(1), SIPTAG_SUPPORTED_STR(""),
			   SIPTAG_CONTENT_TYPE_STR("application/sdp"), SIPTAG_PAYLOAD_STR(sofiaobj->m_sOffSdp.c_str()), TAG_END());

	XGLOG_INFO("SofiaSiphandler::ForwardReInvite Sent out.... %s", sofiaobj->callid.c_str());
}

char *SofiaSiphandler::deblank(char *input)
{
	int i, j;
	char *output = input;
	for (i = 0, j = 0; i < strlen(input); i++, j++)
	{
		if (input[i] != ' ')
			output[j] = input[i];
		else
			j--;
	}
	output[j] = 0;
	return output;
}

void SofiaSiphandler::prepare_url_string(std::string client_ip, std::string client_port, std::string client_transport, std::string &sUrl)
{
	std::string url("");
	url.assign("sip:");
	url.append(client_ip.c_str());
	url.append(":");
	url.append(client_port.c_str());
	url.append(";");
	url.append("transport=");
	url.append(client_transport.c_str());
	sUrl = url.c_str();
}

// Validate IP Address IPV4/IPV6
int SofiaSiphandler::ip_validator(string ipAddress)
{
	struct sockaddr_in sa;
	int result = inet_pton(AF_INET, ipAddress.c_str(), &(sa.sin_addr));

	return result;
}

bool SofiaSiphandler::is_valid_ip_address(string ipAddress)
{
	char delimV6 = ':';
	char delimV4 = '.';
	int countV6 = 0;
	int countV4 = 0;
	for (int i = 0; (i = ipAddress.find(delimV6, i)) != std::string::npos; i++)
	{
		countV6++;
	}

	if (countV6 >= 3)
	{
		return true;
	}

	for (int i = 0; (i = ipAddress.find(delimV4, i)) != std::string::npos; i++)
	{
		countV4++;
	}

	if (countV4 >= 3)
	{
		return true;
	}

	return false;
}

/*
* Validate IPv6 address has '[' or not 
*/
std::string SofiaSiphandler::GetValidIP(std::string ipAddress)
{
	int count = std::count(ipAddress.begin(), ipAddress.end(), ':');
	if (count > 2)
	{
		if (ipAddress.at(0) == '[')
			return ipAddress;
		else
			return '[' + ipAddress + ']';
	}
	return ipAddress;
}

string SofiaSiphandler::convert_uppercase_to_lowercase(const char *transport)
{
	std::string stransport("");
	char buf[strlen(transport)];
	memset(buf, '\0', sizeof(buf));
	for (int i = 0; i < strlen(transport); i++)
	{
		buf[i] = tolower(transport[i]);
	}

	stransport = buf;

	return stransport.c_str();
}

unsigned char hexToChar(const std::string &str)
{
	short c = 0;

	if (!str.empty())
	{
		std::istringstream in(str);

		in >> std::hex >> c;

		if (in.fail())
		{
			// throw std::runtime_error("stream decode failure");
		}
	}
	return static_cast<unsigned char>(c);
}

char *SofiaSiphandlerReplaceHeaderParser::parseReplaceHeader(const char *header)
{
	int counter = 0;
	rest = (char *)header;
	while ((token = strtok_r(rest, "=", &rest)))
	{
		if (strcasecmp(token, "Replaces") == 0 && counter == 0)
		{
			printf("%s\n", token);
			str2 = rest; /*Store rest other string after 1st token*/
			printf("%s\n", str2);
			counter = counter + 1;
		}
		if (rest != nullptr)
		{
			while ((token2 = strtok_r(str2, "%", &str2)))
				if (counter == 1)
				{ /*this condition will give Call Id*/
					replaceHdrCallID = token2;
					counter = counter + 1;
					printf("%s\n", token2);
				}
				else if (counter == 2)
				{ /*this condition will extrat To Tag from ReplaceHeader*/
					replaceHdrToTag = token2 + 2;
					counter = counter + 1;
					printf("%s\n", token2 + 2);
				}
				else if (counter == 3)
				{ /*this condition will give value of To Tag */
					toTageValue = token2;
					counter = counter + 1;
					printf("%s\n", token2 + 2);
				}
				else if (counter == 4)
				{ /*this condition will extract From Tag in Replaces header */
					replaceHdrFrmTag = token2 + 2;
					counter = counter + 1;
					printf("%s\n", token2 + 2);
				}

				else if (counter == 5)
				{ /*this condition will give value from Tag */
					FrmTagValue = token2 + 2;
					counter = counter + 1;
					printf("%s\n", token2 + 2);
				}
				else
				{
				}
		}
	}

	sprintf(replaceHdrCallID, "%s", replaceHdrCallID);
	return (0);
}

/*
 * Starting timer for INVITE Request
 */
void SofiaSiphandler::set_invite_timer(xGateSofiaobj *sobj)
{
	XGLOG_FUNCTION_EN_EX("");
	if (!sobj)
	{
		XGLOG_ERROR("SofiaSiphandler::set_invite_timer Failed sobj is nullptr");
		return;
	}
	if (m_pSipHndlrParams->m_iRespTimeOut > 0)
	{
		xGateCallObj *callObj = new xGateCallObj();
		callObj->uid = sobj->uid.c_str();
		sobj->isInviteTimerStarted = true;
		callObj->iTimerValue = m_pSipHndlrParams->m_iRespTimeOut;
		callObj->sipeventtype = EN_INVITE_SET_TIMER;
		PostMessage(callObj, XGATEPOSTTYPE::EN_PROCESS);
		XGLOG_INFO("SofiaSiphandler::set_invite_timer is started for uid(%s)", sobj->uid.c_str());
		return;
	}
	XGLOG_INFO("SofiaSiphandler::set_invite_timer is failed because start time(%d) less than Zero", m_pSipHndlrParams->m_iRespTimeOut);
	return;
}

/*
 * Cancel the timer request
 */
void SofiaSiphandler::cancel_invite_timer(xGateSofiaobj *sobj)
{
	XGLOG_FUNCTION_EN_EX("");

	if (!sobj)
	{
		XGLOG_ERROR("SofiaSiphandler::cancel_invite_timer Failed sobj is nullptr");
		return;
	}

	xGateCallPair *callPair = nullptr;
	xGateSofiaobj *Tsobj = nullptr;

	callPair = GetConnectedList(sobj->uid);
	if (callPair)
	{
		if (callPair->Orgcall)
			Tsobj = callPair->Orgcall;
		else
			return;
	}
	else
	{
		XGLOG_ERROR("SofiaSiphandler::cancel_invite_timer GetConnectedList callPair is nullptr !");
		return;
	}

	if (Tsobj->isInviteTimerStarted)
	{
		xGateCallObj *callObj = new xGateCallObj();
		callObj->uid = sobj->uid.c_str();
		callObj->sipeventtype = EN_INVITE_CANCEL_TIMER;
		PostMessage(callObj, XGATEPOSTTYPE::EN_PROCESS);
		Tsobj->isInviteTimerStarted = false;
		XGLOG_INFO("SofiaSiphandler::cancel_invite_timer timer canceled for uid(%s)", sobj->uid.c_str());
	}
}
/*
 *Handle the timeout event for Invite and send CANCEL to the particular INVITE
*/

void SofiaSiphandler::timeout_invite_timer(xGateSofiaobj *sobj)
{
	XGLOG_FUNCTION_EN_EX("");
	if (!sobj)
	{
		XGLOG_ERROR("SofiaSiphandler::timeout_invite_timer Failed sobj is nullptr");
		return;
	}

	if(sobj->SofiaLegB->sofianh || isPBXCall((sip_t *)sobj->siptag))
	{
		ResponseWithReasonHeader((nua_handle_t *)sobj->sofianh, 408, "Request Timeout","SIP;cause=408;text=\"Client Connection Timeout\"");
		if (sobj->isInviteTimerStarted)
		{
			SofiaSendCancel((nua_handle_t *)sobj->SofiaLegB->sofianh);
		}
		XGLOG_INFO("SofiaSiphandler::timeout_invite_timer Cancel send to failed Leg!");
	}

#if 0 // Currenty we are rerouting call after timeout
	if (!sobj->isCallReRouted)
	{
		sobj->cp.isServerFailure = true;
		ReRouteCallToOtherNode(sobj);
	}
#endif	
		RespondToPeer(503, sobj);
	}

/*
 * generating Registeration contact
 */
std::string SofiaSiphandler::generate_reg_contact(xGateSofiaSipRegobj *regobj)
{
	if (!regobj)
	{
		XGLOG_ERROR("SofiaSiphandler::generate_reg_contact regobj is nullptr ");
		return "";
	}
	std::string schema(""),contact("");
	if (regobj->transporttype.compare("tls") == 0)
		schema = "sips";
	else
		schema = "sip";
	char contact_hvalue[512];
	memset(contact_hvalue, 0, sizeof(contact_hvalue));

	int iBytes = snprintf(contact_hvalue, sizeof(contact_hvalue), "<%s:%s@%s;transport=%s>;expires=%d",
						  schema.c_str(),
						  regobj->proxyusername.c_str(),
						  regobj->contactaddress.c_str(),
						  regobj->transporttype.c_str(),
						  m_pSipHndlrParams->m_iServerExpires);
	XGLOG_INFO("SofiaSiphandler::generate_reg_contact ");
	if (iBytes >= sizeof(contact_hvalue))
	{
		XGLOG_ERROR("SofiaSiphandler::generate_reg_contact is failed to encode contact header");
		return "";
	}
	contact_hvalue[iBytes] = '\0';
    contact = contact_hvalue;
	return contact;
}

/*
* set responce contact for 18xx and 2xx
*/
void SofiaSiphandler::set_resp_contact(xGateSofiaobj &sobj)
{
	std::string sIPAddress{};
	std::string schema{};
	std::string port{};
	if (sobj.callLegIpAddrFamily == IP_ADDR_V6)
	{
		sIPAddress.assign("[");
		sIPAddress.append(m_pSipHndlrParams->m_sIPv6Add.c_str());
		sIPAddress.append("]");
	}
	else
	{
		sIPAddress = m_pSipHndlrParams->m_sIPv4Add.c_str();
	}

	if (sobj.m_sTransportType.compare("tls")==0)
	{
		schema = "sips";
		port =  m_pSipHndlrParams->m_sSipsPort.c_str();
	}
	else if (sobj.m_sTransportType.compare("wss")==0)
	{
		schema = "sip";
		port = m_pSipHndlrParams->m_sWssPort.c_str();
	}
	else
	{
		schema = "sip";
		port = m_pSipHndlrParams->m_sSipPort.c_str();
	}

	char contact_hvalue[512];
	int iBytes = snprintf(contact_hvalue, 512, "<%s:%s@%s:%s;transport=%s>",
			 schema.c_str(),
			 sobj.cp.cnCalling.c_str(),
			 sIPAddress.c_str(), port.c_str(),
			 sobj.m_sTransportType.c_str());

	if (iBytes >= sizeof(contact_hvalue))
	{
		XGLOG_ERROR("SofiaSiphandler::set_resp_contact is failed to encode contact header");
		return;
	}
	contact_hvalue[iBytes] = '\0';
	sobj.cp.m_sRespContact = contact_hvalue;

}

/*
* check below Private IP Range if not matched then that is public IP 
* 10.x.x.x
* 172.x.x.x
* 192.x.x.x 
*/
bool SofiaSiphandler::is_public_ip(std::string ip)
{
	std::string str1 = "10";
	std::string str2 = "172";
	std::string str3 = "192";
	char str_cstr[ip.length() + 1];
	strcpy(str_cstr, ip.c_str());
	std::string token = strtok(str_cstr, ".");

	if (strcmp(token.c_str(), str1.c_str()) == 0)
		return false;
	else if (strcmp(token.c_str(), str2.c_str()) == 0)
		return false;
	else if (strcmp(token.c_str(), str3.c_str()) == 0)
		return false;

	return true;
}

/*
* Extract Cli and Extension from uri
*/
std::tuple<bool, std::string, std::string> 
SofiaSiphandler::extractCliandExt(std::string url)
{
	size_t found = url.find(";ext=");
	int len = url.length();
	if (found != string::npos)
	{
		return {true, url.substr(0, found), url.substr(found + 5, len)};
	}
	else
	{
		return {false, "", ""};
	}
}

/*
* Check the call is PBX call or not based on User Agent
*/
inline bool SofiaSiphandler::isPBXCall(sip_t const *sip)
{
	if (sip && sip->sip_user_agent && sip->sip_user_agent->g_string)
	{
		if (strncmp(sip->sip_user_agent->g_string, m_pSipHndlrParams->m_sPbxUA.c_str(), m_pSipHndlrParams->m_sPbxUA.length()) == 0)
			return true;
	}
	return false;
}
