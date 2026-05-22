#include "BSGmsc.h"
#include "LogHandler.h"
#include "BusinessServer.h"
#include "BSHttpService.h"

#define APP "GMSC:"

BSGmsc::BSGmsc(BSParam* bsp, Application app)
{
    if(bsp)
	{
        m_strSiteCode = bsp->getSiteCode();
        m_strSwitchCode = bsp->getSwitchCode();
		m_strCli = bsp->getCli();
    	m_strDdi = bsp->getDdi();
		m_strTrunkName = bsp->getTrunkName();
		m_strIwmscPrefix = bsp->getIwmscPrefix();
		m_strMvnoPrefix = bsp->getMvnoPrefix();
        m_strDBProfile = app.getHlrDB();
        m_strDBMnpProfile = app.getMnpDB();
		m_strCRF =  bsp->getUid();
		local_ip_address = GetLocalIPAddress(AF_INET);
        m_strHlrUrl = bsp->getHlrUrl();
		m_strAppID =  generateAppID();
		setAppType(APPLICATIONTYPE::TYPE_GMSC);
		bsp->setInterTrunkGroup("PIPER");
        m_datUpdate = new BSDAT();		
        m_datUpdate->setSessionId(m_strCRF);
        m_datUpdate->setCli(m_strCli);
        m_datUpdate->setDdi(m_strDdi);
		m_datUpdate->setSwitchCode(std::to_string(m_strSwitchCode));
		m_datUpdate->setSiteCode(m_strSiteCode.c_str());
		m_datUpdate->setTrunkName(m_strTrunkName);
		m_datUpdate->setFileFormat(".DAT");
		setBsRequestTime();
    }
	else
	{
        // throw exception objection creation failure;
    }
}


BSGmsc::~BSGmsc(){

        if(m_datUpdate){
                delete m_datUpdate;
                m_datUpdate = NULL;
        }
}


void BSGmsc::postToDAT()
{
    std::unique_ptr<BSDAT> bsdatMsg(new BSDAT);
    if(bsdatMsg)
    {
        bsdatMsg->setSessionId(m_datUpdate->getSessionId());
        bsdatMsg->setSiteCode(m_datUpdate->getSiteCode());
        bsdatMsg->setSwitchCode(m_datUpdate->getSwitchCode());
        bsdatMsg->setCli(m_datUpdate->getCli());
        bsdatMsg->setDdi(m_datUpdate->getDdi());
	bsdatMsg->setTrunkName(m_datUpdate->getTrunkName());
	bsdatMsg->setFileFormat(m_datUpdate->getFileFormat());
	bsdatMsg->setLegAdev(m_datUpdate->getLegAdev());
	bsdatMsg->setLegBdev(m_datUpdate->getLegBdev());
        bsdatMsg->setMsisdn(m_datUpdate->getMsisdn());
        bsdatMsg->setIMSI(m_datUpdate->getIMSI());
        bsdatMsg->setMsrn(m_datUpdate->getMsrn());
        bsdatMsg->setAppDur(m_datUpdate->getAppDur());
	bsdatMsg->setTalkTime(m_datUpdate->getTalkTime());
        bsdatMsg->setDisconnectReqBy(m_datUpdate->getDisconnectReqBy());
        bsdatMsg->setFwdType(m_datUpdate->getFwdType());
	bsdatMsg->setTrunkOut(m_datUpdate->getTrunkOut());
	bsdatMsg->setLegBConnect(m_datUpdate->getLegBConnect());
	bsdatMsg->setLegAConnect(m_datUpdate->getLegAConnect());
        bsdatMsg->setCallCause(m_datUpdate->getCallCause());
        bsdatMsg->setFwDdi(m_datUpdate->getFwDdi());
        bsdatMsg->setConnectionState(m_datUpdate->getConnectionState());
	bsdatMsg->setTalkTime(m_datUpdate->getTalkTime());
        bsdatMsg->setVlr(m_datUpdate->getVlr());
        bsdatMsg->setMsc(m_datUpdate->getMsc());
        bsdatMsg->setMsgType(MESSAGEEVENT::MSG_GMSC2REP);
        BusinessServer::post(BUSINESS_THREADS::MVNO_BSREP, std::move(bsdatMsg));
    }
}

retCode BSGmsc::Load(CA::CAMap<CA::DBInterface*>& dblist, BSParam* bsp, Application app)
{
    logdebug(m_strCRF,"BSGmsc::Load() CLI %s DDI %s",m_strCli.c_str(),m_strDdi.c_str());
    if (dblist.GetMapSize() == 0)
    {
        setLastKnownError("DB Connection list empty");
        logerror(m_strCRF,"BSGmsc::Load()%s",getLastKnownError().c_str());
        return retCode::BS_ERROR_UNKOWN;
    }
    if(bsp == nullptr)
    {
        setLastKnownError("CallIn param object is NULL");
        logerror(m_strCRF,"BSGmsc::Load()%s",getLastKnownError().c_str());
        return retCode::BS_NULL_OBJECT;
    }
	logdebug(m_strCRF,"BSGmsc::Load() [DB Profile] %s",m_strDBProfile.c_str() );
    retCode ret =  retCode::BS_SUCCESS;
    CA::DBInterface* dbInt = dblist.FindValue(m_strDBProfile);
	CA::DBInterface* dbMnpInt = dblist.FindValue(m_strDBMnpProfile);

    //xlat in for A Party
	m_xlate_in_legA = "";
	m_xlate_in_legA = XlatIn(dbInt, bsp, 1);
	m_datUpdate->setLegAdev("rtp7");
	m_datUpdate->setTrunkOut("piper");
	m_datUpdate->setLegBdev("piper");
	bsp->setCli(m_xlate_in_legA.c_str());
	
	//xlat in for B Party
	m_xlate_in_legB = "";
	m_xlate_in_legB =XlatIn(dbInt, bsp, 2);
	bsp->setDdi(m_xlate_in_legB.c_str());
	
	the_subs.msisdn =  DDI2MSISDN(dbInt, m_xlate_in_legB).c_str();
	m_datUpdate->setMsisdn(the_subs.msisdn);
	std::string errMsg = "";
	SubscriberStatus subs_status = checkSubscriber(dbInt, errMsg);

	if(subs_status == SubscriberStatus::SUBS_STATUS_UNKNOWN)
	{
		if(checkNonSubscriberNB(dbInt, dbMnpInt, the_subs.msisdn)==retCode::BS_SUCCESS)
		{
			ConnectNonSubs(dbInt, bsp);
			bsp->setCliAddr(local_ip_address + ":5060");
			std::unique_ptr<BSRouting> rsInterface (new BSRouting(bsp,app));
    		m_rsInterface =  std::move(rsInterface);
    		return  m_rsInterface->InitiateRouting(dblist,bsp);
			
		}
		else
		{
			if(multinb.flag == 1 && multinb.errcode != 0 && !multinb.prompt.empty())
			{
				loginfo(m_strCRF,"Subscriber Error Multi Number: %s",multinb.errmsg.c_str());
				return retCode::BS_SUB_MULTI_NUMBER_ERROR;
			}
			else
			{
				loginfo(m_strCRF,"Subscriber Error  %s",errMsg.c_str());
				return retCode::BS_SUB_ERROR;
			}
			
		}
	}
	if(multinb.flag == 1)
	{
		if(multinb.errcode != 0)
		{
			loginfo(m_strCRF, "gmsc MulitiNB Error Code : %d Msg %s ",multinb.errcode,multinb.errmsg.c_str());
			if(!multinb.ussdmsg.empty())
			{
			// Post for USSD 
			}
			return retCode::BS_SUB_MULTI_NUMBER_ERROR;
		}
	}
	// check for Sim based routing
	if(SIMBasedRouting(dbInt, bsp, the_subs.MSRN)==retCode::BS_SUCCESS)
	{
		bsp->setDdi(the_subs.MSRN.c_str());
		if(Connect(dbInt, bsp)==retCode::BS_SUCCESS)
		{
			bsp->setCliAddr(local_ip_address + ":5060");
			ret =  retCode::BS_SUCCESS_DIRECTDIAL;
		}
	}
	else
	{
		//Call Forwarding  need to implemet
		CallForward(dbInt, bsp);
		if(fwd_Uncond.state)
		{
			loginfo(m_strCRF, "Call Forward Unconditional to:%s",fwd_Uncond.ftn_nb.c_str());
			fwd_type =  (int)CallForwardType::CFWD_CFU; // for CDR
			m_datUpdate->setFwdType(fwd_type);
			ret = setForward(dbInt, bsp, fwd_Uncond);
		}
		else
		{
			switch(subs_status)
			{
				case SubscriberStatus::SUBS_STATUS_PURGED:
				{}
				case SubscriberStatus::SUBS_STATUS_NOTACTIVE:
				{
					if (fwd_NoReach.state)
					{
						logerror(m_strCRF, "Call Forward On NoReach to:%s",fwd_NoReach.ftn_nb.c_str());
						fwd_type = (int)CallForwardType::CFWD_CFNRC;	// for CDR
						m_datUpdate->setFwdType(fwd_type);
						ret = setForward(dbInt, bsp, fwd_NoReach);
					}
					else
					{
						logerror(m_strCRF, "checkSubscriber Error result status and  state not active  :%s in fwd_NoReach",fwd_NoReach.ftn_nb.c_str());
						ret = retCode::BS_EXCHANGE_ROUTING_ERROR;
					}
				}
				break;
				case SubscriberStatus::SUBS_STATUS_ACTIVE:  
				{
					if(the_subs.isCBblock)
					{
						if(fwd_Busy.state)
						{
							logerror(m_strCRF, "Call Forward On Busy to:%s",fwd_Busy.ftn_nb.c_str());
							fwd_type = (int)CallForwardType::CFWD_CFB; // for CDR
							m_datUpdate->setFwdType(fwd_type);
							ret = setForward(dbInt, bsp, fwd_Busy);
						}
						else
						{
							logerror(m_strCRF, "checkSubscriber Error result status and  state not active  :%s in fwd_Busy",fwd_Busy.ftn_nb.c_str());
							ret = retCode::BS_ERROR_UNKOWN;
						}
					}
					else
					{
						ret = HLRRequest(bsp);
					}
				}
				break;
				case SubscriberStatus::SUBS_STATUS_BLOCKED:
				{
					loginfo(m_strCRF, "SubscriberError: Subscriber blocked. MSISDN:%s",bsp->getDdi().c_str());
					ret = retCode::BS_ABSENT_SUBSCRIBER;
				}
				break;
				default:
				{
					loginfo(m_strCRF, "SubscriberError: unknown error MSISDN:%s",bsp->getDdi().c_str());
					ret = retCode::BS_ERROR_UNKOWN;
				}
				break;
			}
		}
	}
	switch(ret){
		case retCode::BS_SUCCESS_DIRECTDIAL:
		case retCode::BS_SUCCESS:
		// Routing 
    	std::unique_ptr<BSRouting> rsInterface (new BSRouting(bsp,app));
    	m_rsInterface =  std::move(rsInterface);
    	ret =  m_rsInterface->InitiateRouting(dblist,bsp);
		break;

	}
	return ret;
}

retCode BSGmsc::HLRRequest(BSParam *bsp)
{
	logdebug(m_strCRF, "BSGmsc::HLRRequest called");
	retCode ret = retCode::BS_SUCCESS;
	std::string url(""), header(""), body("");
	
	if(m_strHlrUrl.compare("http://localhost/hlr") == 0){
		url = "http://" + local_ip_address + "/hlr";
	}else{
		url = m_strHlrUrl;
	}
	
	header = "application/x-www-form-urlencoded";
	body = "imsi="+the_subs.imsi+"&msisdn="+the_subs.msisdn +"&msc="+the_subs.msc + "&vlr="+the_subs.vlr;
	loginfo(m_strCRF, "HLR Http Request %s %s %s",url.c_str(), header.c_str(),body.c_str());
	std::unique_ptr<BSHttpServiceMsg> httpMsg (new BSHttpServiceMsg());
	if(httpMsg)
	{
		httpMsg->setUrl(url);
		httpMsg->setHeader(header);
		httpMsg->setBody(body);
		httpMsg->setContextId(bsp->getUid());
		m_pHttpMsg =  std::move(httpMsg);
		//httpMsg->setMsgType(MESSAGEEVENT::MSG_ROUT2HTTP);
		//BusinessServer::post(BUSINESS_THREADS::MVNO_HTTP,std::move(httpMsg));
		loginfo(m_strCRF, "Suspended for HLR Request %s",bsp->getUid().c_str());
		ret =  retCode::BS_HLR_REQUEST;
	}
	else
	{
		ret = retCode::BS_NULL_OBJECT;
	}
	return ret;
}

//retCode BSGmsc::HLRResponse(CA::CAMap<CA::DBInterface*>& dblist,BSOperation *bsOpr, BSParam *bsp)
retCode BSGmsc::HLRResponse(CA::CAMap<CA::DBInterface*>& dblist, BSParam *bsp,int iHttpStatusCode,std::string strHttpOutput, Application app)
{
	CA::DBInterface* dbInt = dblist.FindValue(m_strDBProfile);
	logdebug(m_strCRF, "BSGmsc::HLRResponse called");
	retCode ret = retCode::BS_ERROR_UNKOWN;
	
	if(iHttpStatusCode != 200)
	{
		loginfo(m_strCRF, "BSGmsc::HLRResponse %s Disconnecting on HLR Request fail",bsp->getUid().c_str());
		return ret;
	}
	if(strHttpOutput.compare("")==0 || strHttpOutput.empty())
	{
		loginfo(m_strCRF, "BSGmsc::HLRResponse No Data from HLR for callid %s  data %s ",bsp->getUid().c_str(),strHttpOutput.c_str());
		return ret;
	}
	char hlrMsgCode[4] = {0};
	char hlrMsg[50] = {0};
	sscanf(strHttpOutput.c_str(),"%[^:]:%s",hlrMsgCode,hlrMsg);
	if(strlen(hlrMsgCode) <= 0)
	{
		sprintf(hlrMsgCode,"-1");
	}
	try
	{
		if(std::stoi(hlrMsgCode) == 0 )
		{
			loginfo(m_strCRF, "BSGmsc::HLRResponse Got Data %s",strHttpOutput.c_str());
			if(strlen(hlrMsg) > 0)
			{
				the_subs.msrn = hlrMsg;
				m_datUpdate->setMsrn(the_subs.msrn.c_str());
				bsp->setDdi(the_subs.msrn.c_str());
				ret = Connect(dbInt, bsp);
			}
		}else
		{
			switch(std::stoi(hlrMsgCode))
			{
				case 27: // mobile  switch off
				{
					logerror(m_strCRF, "SubscriberError 27 : %s",hlrMsg);
					ret = retCode::BS_ABSENT_SUBSCRIBER;
				}
				break;
				case 13:
				{
					logerror(m_strCRF, "SubscriberError 13 : %s",hlrMsg);
					ret = retCode::BS_OUTGOING_BARRED;
				}
				break;
				case -1:
				{
					logerror(m_strCRF, "SubscriberError -1 : %s",hlrMsg);
					ret = retCode::BS_NO_ROUTE;
				}
				break;
				default:{
					loginfo(m_strCRF, "BSGmsc::HLRResponse no Data found %s data %s", bsp->getUid().c_str(),strHttpOutput.c_str());
					ret = retCode::BS_HLR_FAILED;
				}
				break;
			}
		}
	}
	catch(std::invalid_argument& e)
	{
		ret = retCode::BS_HLR_FAILED;
	}
	catch(...)
	{
		ret = retCode::BS_HLR_FAILED;
	}

	switch(ret){
		case retCode::BS_SUCCESS_DIRECTDIAL:
		case retCode::BS_SUCCESS:
		// Routing 
    	std::unique_ptr<BSRouting> rsInterface (new BSRouting(bsp,app));
    	m_rsInterface =  std::move(rsInterface);
    	ret =  m_rsInterface->InitiateRouting(dblist,bsp);
		break;

	}
	
	return ret;
}

retCode BSGmsc::CallForward(CA::DBInterface *dbInt, BSParam *bsp)
{
	logdebug(m_strCRF, "BSGmsc::CallForward called");
	std::string spName(""), spValue("");
	spName = "gmsc_get_forwarding";
	spValue = " '" + the_subs.imsi + "'";
	
	logdebug(m_strCRF,"DB gmsc_get_forwarding Query = %s %s", spName.c_str(), spValue.c_str());
	if(!dbInt)
	{
        logerror(m_strCRF,"BSGmsc::DB_gmsc_get_forwarding() DB Connection failure");
        return retCode::BS_DB_CONNECTION_FAILED;
    }
	CA::DBMAP m_dbCallFwd;
	retCode retcode = executeQuery(dbInt,spName,spValue,m_dbCallFwd);
    if(retcode != retCode::BS_SUCCESS)
	{
        logerror(m_strCRF, "Error On gmsc_get_forwarding GetDBDetails");
    }
	if(m_dbCallFwd.size() == 0)
	{
        setLastKnownError("No SQL Record Found");
    }
	if(m_dbCallFwd.size() > 0)
	{
		int sscode,q=0,p=0,r=0,a=0;
		for (int i = 0; i < m_dbCallFwd.size() ; i++){
			getData("ss_code",i,m_dbCallFwd,sscode);
			getData("q",i,m_dbCallFwd,q);
			getData("p",i,m_dbCallFwd,p);
			getData("r",i,m_dbCallFwd,r);
			getData("a",i,m_dbCallFwd,a);
			switch((CallForwardType)sscode)
			{
				case CallForwardType::CFWD_CFU:
				{ // unconditional fwd
					if(q == 0 && p == 1 && r == 1 && a == 1)
					{
						fwd_Uncond.state = TRUE;
						getData("ftn_address",i,m_dbCallFwd,fwd_Uncond.ftn_nb);
						getData("ftn_noa",i,m_dbCallFwd,fwd_Uncond.ftn_noa);
						getData("max_nry",i,m_dbCallFwd,fwd_Uncond.max_nry);
					}
					else
					{
						fwd_Uncond.state = FALSE;
					}	
				}
				break;
				case CallForwardType::CFWD_CFB:
				{
					if(q == 0 && p == 1 && r == 1 && a == 1)
					{
						fwd_Busy.state = TRUE;
						getData("ftn_address",i,m_dbCallFwd,fwd_Busy.ftn_nb);
						getData("ftn_noa",i,m_dbCallFwd,fwd_Busy.ftn_noa);
						getData("max_nry",i,m_dbCallFwd,fwd_Busy.max_nry);
					}
					else
					{
						fwd_Busy.state = FALSE;
					}
				}
				break;
				case CallForwardType::CFWD_CFNRY:
				{
					if(q == 0 && p == 1 && r == 1 && a == 1)
					{
						fwd_NoReply.state = TRUE;
						getData("ftn_address",i,m_dbCallFwd,fwd_NoReply.ftn_nb);
						getData("ftn_noa",i,m_dbCallFwd,fwd_NoReply.ftn_noa);
						getData("max_nry",i,m_dbCallFwd,fwd_NoReply.max_nry);
					}
					else
					{
						fwd_NoReply.state = FALSE;
					}
				}
				break;
				case CallForwardType::CFWD_CFNRC:
				{
					if(q == 0 && p == 1 && r == 1 && a == 1)
					{
						fwd_NoReach.state = TRUE;
						getData("ftn_address",i,m_dbCallFwd,fwd_NoReach.ftn_nb);
						getData("ftn_noa",i,m_dbCallFwd,fwd_NoReach.ftn_noa);
						getData("max_nry",i,m_dbCallFwd,fwd_NoReach.max_nry);
					}
					else
					{
						fwd_NoReach.state = FALSE;
					}
				}
				break;
				default:
				{
				}
				break;
			}
		}
	}
	return retCode::BS_SUCCESS;
}

retCode BSGmsc::setForward(CA::DBInterface *dbInt, BSParam *bsp, RForwarding fwd)
{
	logdebug(m_strCRF, "BSGmsc::setForward called");
	bsp->setRedirectingNumber(the_subs.msisdn.c_str());
	bsp->setRN(the_subs.msisdn.c_str());
	m_datUpdate->setFwDdi(bsp->getRedirectingNumber());
	//bsp->setRedirectingNumber(the_subs.msisdn.c_str());
	//bsp->setCntRedirectingNumber(CallNumberType::CNT_INTERNATIONAL);
	std::string xout = XlatOut(dbInt, bsp, m_strIwmscPrefix + fwd.ftn_nb).c_str();
	bsp->setDdi(xout.c_str());
	switch(fwd.ftn_noa)
	{
		case 0:
		{
			m_enCntCalled = (CallNumberType)0;
		}
		break;
		case 1:
		{
			m_enCntCalled = (CallNumberType)4;
		}
		break;
		case 2:
		{}
		break;
		case 3:
		{
			m_enCntCalled = (CallNumberType)2;
		}
		break;
		case 4:
		{
			m_enCntCalled = (CallNumberType)1;
		}
		break;
		default:
		{
			m_enCntCalled = (CallNumberType)0;
		}
		break; 
	}
	loginfo(m_strCRF, "Forward Call to :%s",bsp->getDdi().c_str());
	return retCode::BS_SUCCESS;
}

retCode BSGmsc::Connect(CA::DBInterface *dbInt, BSParam* bsp)
{
	logdebug(m_strCRF, "BSGmsc::Connect called");
	
	if(!bsp){
		return:: retCode::BS_NULL_OBJECT;
	}
	std::string mode = "5";
	if(the_subs.isRoaming == TRUE)
	{
		mode = "6";
	}
	/*bsp->getCpInfo().clear();
	bsp->setCpInfo(std::string("MSISDN-")+the_subs.msisdn+":CALLMODE-"+mode);
	if(strstr(bsp->getPrivateInfo().c_str(),"CB-"))
	{
		bsp->setState(BSCallState::STATE_PROCESSING);	
		return retCode::BS_SUCCESS;
	}*/
	//bsp->setState(BSCallState::STATE_PROCESSING);
	bsp->setDdi(XlatOut(dbInt, bsp, m_strMvnoPrefix + bsp->getDdi()));
	if(multinb.flag == 1 && multinb.prefixdest != "" && !isRoaming)
	{
		bsp->setDdi(multinb.prefixdest + bsp->getDdi());
	}
	return retCode::BS_SUCCESS;
}

retCode BSGmsc::SIMBasedRouting(CA::DBInterface* dbInt, BSParam* bsp, std::string &simbased_nb)
{
	logdebug(m_strCRF, "BSGmsc::SIMBasedRouting called");
	retCode ret = retCode::BS_ERROR_UNKOWN;
	std::string spName(""), spValue("");
	int h(0), simbased_flag(0);
	spName = "gmsc_get_simbased_routing";
	//spValue = " '" + bsp->getTrunkName() + "','" + bsp->getDdi() + "'";
	spValue = " ' ', '" + bsp->getDdi() + "'";
	logdebug(m_strCRF,"DB gmsc_get_simbased_routing Query = %s %s", spName.c_str(), spValue.c_str());
	if(!dbInt)
	{
        logerror(m_strCRF,"BSGmsc::DB_gmsc_get_simbased_routing() DB Connection failure");
        return retCode::BS_DB_CONNECTION_FAILED;
    }
	CA::DBMAP m_dbSimBased;
	retCode retcode = executeQuery(dbInt,spName,spValue,m_dbSimBased);
    if(retcode != retCode::BS_SUCCESS)
	{
        logerror(m_strCRF, "Error On gmsc_get_simbased_routing GetDBDetails");
    }
	if(m_dbSimBased.size() == 0)
	{
        setLastKnownError("No SQL Record Found");
    }
	if(m_dbSimBased.size() > 0)
	{
		getData("simbased_flag",0,m_dbSimBased,simbased_flag);
	}
	if(simbased_flag == 1)
    {   
		simbased_nb = XlatBySpecialRouting(dbInt, bsp, the_subs.MSISDN);
		if(simbased_nb != the_subs.MSISDN )
		{
			ret = retCode::BS_SUCCESS;		
		}
    }	 
  	return ret;
}

std::string BSGmsc::XlatBySpecialRouting(CA::DBInterface* dbInt, BSParam* bsp, std::string &nb)
{
	logdebug(m_strCRF, "BSGmsc::XlatBySpecialRouting called");
	std::string ret = nb.c_str();
	isNeedDeleteSR = false;
	int routing_id = 0;
	std::string spName(""), spValue("");
	spName = "gmsc_getaccount_mvno";
	spValue = " '" + the_subs.msisdn + "'";
	logdebug(m_strCRF,"DB gmsc_getaccount_mvno Query = %s %s", spName.c_str(), spValue.c_str());
	if(!dbInt)
	{
        logerror(m_strCRF,"BSGmsc::XlatBySpecialRouting() DB Connection failure");
        return ret;
    }
	CA::DBMAP dbgetAccMvno;
	retCode retcode = executeQuery(dbInt,spName,spValue,dbgetAccMvno);
    if(retcode != retCode::BS_SUCCESS)
	{
        logerror(m_strCRF, "Error On gmsc_getaccount_mvno GetDBDetails");
    }
	if(dbgetAccMvno.size() == 0)
	{
        setLastKnownError("No SQL Record Found");
    }
	if(dbgetAccMvno.size() > 0)
	{
		getData("in_routing_id",0,dbgetAccMvno,routing_id);
	}
	if (routing_id == 0)
	{
		loginfo(m_strCRF, "XlatBySpecialRouting routing _id = 0 Exit %s",bsp->getUid());
		return ret;
	}
	// Updating the Transaction
	spName = "gmsc_simbasedpfx_transaction";
	spValue = " '" + m_strSiteCode + "','" + m_xlate_in_legA + "','" + the_subs.msisdn + "','" + bsp->getUid() + "','" + std::to_string(m_strSwitchCode) + "','" + bsp->getTrunkName() + "'";
	int errCode(-1);
  	std::string errMsg("");
	logdebug(m_strCRF,"DB gmsc_simbasedpfx_transaction Query = %s %s", spName.c_str(), spValue.c_str());
	CA::DBMAP dbupdateTrans;
	retcode = executeQuery(dbInt,spName,spValue,dbupdateTrans);
    if(retcode != retCode::BS_SUCCESS)
	{
        logerror(m_strCRF, "Error On gmsc_simbasedpfx_transaction GetDBDetails");
    }
	if(dbupdateTrans.size() == 0)
	{
        setLastKnownError("No SQL Record Found");
    }
	if(dbupdateTrans.size() > 0)
	{
		getData("errcode",0,dbupdateTrans,errCode);
		getData("errmsg",0,dbupdateTrans,errMsg);
	}
	if(errCode != 0)
	{
		loginfo(m_strCRF, "XlatBySpecialRouting SIM Updation Error Exit %s",bsp->getUid());
		return ret;
  	}
	isNeedDeleteSR = true;
	std::string add_prefix("") ,prefix("");
	spName = "gmsc_get_addprefix";
	spValue = " '" + the_subs.msisdn + "'," + std::to_string(routing_id);
	logdebug(m_strCRF,"DB gmsc_get_addprefix Query = %s %s", spName.c_str(), spValue.c_str());
	CA::DBMAP dbSimPrefix;
	retcode = executeQuery(dbInt,spName,spValue,dbSimPrefix);
    if(retcode != retCode::BS_SUCCESS)
	{
        logerror(m_strCRF, "Error On gmsc_get_addprefix GetDBDetails");
    }
	if(dbSimPrefix.size() == 0)
	{
        setLastKnownError("No SQL Record Found");
    }
	if(dbSimPrefix.size() > 0)
	{
		getData("addprefix",0,dbSimPrefix,add_prefix);
		getData("prefix",0,dbSimPrefix,prefix);
	}
	ret =  XlatConcat(the_subs.msisdn, prefix, add_prefix).c_str();
	loginfo(m_strCRF, "XlatBySpecialRouting Exit %s",bsp->getUid());
	return ret;
}

std::string BSGmsc::XlatIn(CA::DBInterface* dbInt, BSParam* bsp, int type)
{
    logdebug(m_strCRF,"BSGmsc::XlatIn called");
    int h=0;
    std::string ret = "";
    std::string spName(""), spValue(""), pfx(""), xin(""), xout("");
    spName = "gmsc_xlat_in";
    spValue = " '"+m_strSiteCode+"'"+ ",'"+std::to_string(m_strSwitchCode)+"'"+ ",'"+bsp->getTrunkName().c_str()+"',";
	char Query[1024]= {0};
	if(!dbInt)
	{
        logerror(m_strCRF,"BSGmsc::DB_gmsc_xlat_in() DB Connection failure");
        return ret;
    }
    if(type == 1)
    {
         //spValue = spValue + "'" + bsp->getCli() + "'," + std::to_string((int)m_enCntCalling);
         spValue = spValue + "'" + bsp->getCli() + "'," + std::to_string(1); // hardcoded the value
    }
    else
    {
        //spValue = spValue + "'" + bsp->getDdi() + "'," + std::to_string((int)m_enCntCalled);
        spValue = spValue + "'" + bsp->getDdi() + "'," + std::to_string(1); // Hardcode the value
    }
	logdebug(m_strCRF,"DB gmsc_xlat_in Query = %s %s", spName.c_str(), spValue.c_str());
	CA::DBMAP m_dbxlatin;
	retCode retcode = executeQuery(dbInt,spName,spValue,m_dbxlatin);
    if(retcode != retCode::BS_SUCCESS)
	{
		logerror(m_strCRF, "DB gmsc_xlat_in Query failed %s",getLastKnownError().c_str());
		return (type == 1 ) ? bsp->getCli() : bsp->getDdi();
    }
    if(m_dbxlatin.size() == 0)
	{
        setLastKnownError("No SQL Record Found");
		return (type == 1 ) ? bsp->getCli() : bsp->getDdi();
    }
    getData("pfx",0,m_dbxlatin,pfx);
   	getData("xlatin",0,m_dbxlatin,xin);
    logdebug(m_strCRF,"BSGmsc::XlatIn  prefix %s xLatIn %s", pfx.c_str(), xin.c_str());
	std::string nb("");
	if(type == 1)
	{
		nb = bsp->getCli().c_str();
	}
	else
	{
		nb = bsp->getDdi().c_str();
	}
	ret = XlatConcat(nb, pfx, xin).c_str();
	logdebug(m_strCRF,"BSGmsc::XlatIn exit");
	return ret;
}

std::string BSGmsc::XlatConcat(std::string nb, std::string pfx, std::string xin)
{
	logdebug(m_strCRF, "BSGmsc::XlatConcat called");
	std::string ret;
	trim(nb);
	trim(pfx);
	trim(xin);
	try
	{
		if( pfx.compare("") == 0)
		{
			ret = xin + nb;
		}
		else if(pfx.compare(nb) == 0)
		{
			ret = xin.c_str();
		}
		else
		{
			ret =  xin + nb.substr(pfx.length(),(nb.length()-pfx.length()));
		}
	}
	catch(...)
	{
		ret = ""; 
	}
	logdebug(m_strCRF, "BSGmsc::XlatConcat exit");
	return ret;
}

std::string BSGmsc::XlatMNP(CA::DBInterface* dbMnpInt, std::string &np, int &is_onward, int &is_ported)
{ 
	logdebug(m_strCRF, "BSGmsc::XlatMNP called");
	std::string spName(""), spValue(""), ret("");
	char Query[1024] = {0};
	spName = "MNP_get_info";
	spValue = " '"+np+"'";
	logdebug(m_strCRF,"DB MNP_get_info Query = %s %s", spName.c_str(), spValue.c_str());
	if(!dbMnpInt)
	{
        logerror(m_strCRF,"BSGmsc::XlatMNP() DB Connection failure");
        return ret;
    }
	CA::DBMAP m_dbxlatMNP;
	retCode retcode = executeQuery(dbMnpInt,spName,spValue,m_dbxlatMNP);
    if(retcode != retCode::BS_SUCCESS)
	{
		logerror(m_strCRF, "Error on MNP_get_info");
    }
    if(m_dbxlatMNP.size() == 0)
	{
        setLastKnownError("No SQL Record Found");
    }
	else
	{
		getData("mnp_number",0,m_dbxlatMNP,ret);
		getData("is_onward",0,m_dbxlatMNP,is_onward);
		getData("portedflag",0,m_dbxlatMNP,is_ported);
	}
	return ret;
}

std::string BSGmsc::DDI2MSISDN(CA::DBInterface* dbInt, std::string ddi)
{
	logdebug(m_strCRF, "BSGmsc::DDI2MSISDN called");
	std::string spName(""), spValue(""), strtime(""), msisdn("");
	int h;
	msisdn = ddi.c_str();
	strtime = GetTime();
	spName = "gmsc_get_mapped_msisdn_v2";
	spValue = " '"+m_strSiteCode+"','"+ddi+"','"+strtime + "'";
	if(!dbInt)
	{
        logerror(m_strCRF,"BSGmsc::DB_gmsc_get_mapped_msisdn_v2() DB Connection failure");
        return msisdn;
    }
	logdebug(m_strCRF,"DB gmsc_get_mapped_msisdn_v2 Query = %s %s", spName.c_str(), spValue.c_str());
	CA::DBMAP m_dbdd2msisdn;
	retCode retcode = executeQuery(dbInt,spName,spValue,m_dbdd2msisdn);
    if(retcode != retCode::BS_SUCCESS)
	{
		multinb.flag =  0;
        return msisdn;
    }
    if(m_dbdd2msisdn.size() == 0)
	{
        setLastKnownError("No SQL Record Found");
		multinb.flag =  0;
		return msisdn;
    }
	multinb.flag =  1;
    getData("msisdn",0,m_dbdd2msisdn,msisdn);
   	getData("errmsg",0,m_dbdd2msisdn,multinb.errmsg);
	getData("prompt_flag",0,m_dbdd2msisdn,multinb.prompt);
	getData("ussd_msg",0,m_dbdd2msisdn,multinb.ussdmsg);
	getData("prefix_dest",0,m_dbdd2msisdn,multinb.prefixdest);
	getData("errcode",0,m_dbdd2msisdn,multinb.errcode);

	logdebug(m_strCRF, "BSGmsc::DDI2MSISDN exit");
	return msisdn;
}

std::string BSGmsc::XlatOut(CA::DBInterface* dbInt, BSParam *bsp , std::string nb)
{
	logdebug(m_strCRF, "BSGmsc::XlatOut called");
	int h;
	std::string spName(""), spValue(""), ret(""), pfx(""), xout("");
	ret = nb.c_str();
	spName = "gmsc_xlat_out";
	spValue = " '"+ m_strSiteCode + "','" + std::to_string(m_strSwitchCode) + "','"+ bsp->getInterTrunkgroup() + "','" + nb + "'";
	logdebug(m_strCRF,"DB gmsc_xlat_out Query = %s %s", spName.c_str(), spValue.c_str());
	if(!dbInt)
	{
        logerror(m_strCRF,"BSGmsc::DB_gmsc_xlat_out() DB Connection failure");
        return ret;
    }
	CA::DBMAP m_dbxlatout;
	retCode retcode = executeQuery(dbInt,spName,spValue,m_dbxlatout);
    if(retcode != retCode::BS_SUCCESS)
	{
        return ret;
    }
    if(m_dbxlatout.size() == 0)
	{
        setLastKnownError("No SQL Record Found");
		return ret;
    }
	else
	{
		getData("pfx",0,m_dbxlatout,pfx);
		getData("xlatout",0,m_dbxlatout,xout);
		ret = XlatConcat(nb,pfx,xout).c_str();
	}
	return ret;
}

retCode BSGmsc::ConnectNonSubs(CA::DBInterface* dbInt, BSParam *bsp)
{
	logdebug(m_strCRF, "BSGmsc::ConnectNonSubs called");
	if(nonsubs_type == NonSubsType::NONSUBS_ONWARD)
	{
		bsp->setDdi(XlatOut(dbInt, bsp, the_subs.msisdn).c_str());
	}
	else
	{
		bsp->setDdi(XlatOut(dbInt, bsp, m_strIwmscPrefix + the_subs.msisdn).c_str());
	}
	return retCode::BS_SUCCESS;
}

SubscriberStatus BSGmsc::checkSubscriber(CA::DBInterface* dbInt, std::string &errmsg)
{
	logdebug(m_strCRF, "BSGmsc::checkSubscriber called");
	char Query[1024] = {0};
	SubscriberStatus ret = SubscriberStatus::SUBS_STATUS_UNKNOWN;
	errmsg = "Subscriber is not found. MSISDN=" +the_subs.msisdn;
	std::string spName(""), spValue("");
	spName = "gmsc_get_subscriber";
	spValue = " '"+the_subs.msisdn+"'";
	logdebug(m_strCRF, "DB gmsc_get_subscriber Query = %s %s", spName.c_str(), spValue.c_str());
	if(!dbInt)
	{
        logerror(m_strCRF,"BSGmsc::DB_gmsc_get_subscriber() DB Connection failure");
        return ret;
    }
	CA::DBMAP m_dbgetSub;
	retCode retcode = executeQuery(dbInt,spName,spValue,m_dbgetSub);
    if(retcode != retCode::BS_SUCCESS)
	{
		logerror(m_strCRF, "BSGmsc::checkSubscriber Error on gmsc_get_subscriber %s",getLastKnownError().c_str());
    }
    if(m_dbgetSub.size() == 0)
	{
        setLastKnownError("No SQL Record Found");
		return ret;
    }
	getData("isroaming",0,m_dbgetSub,the_subs.isRoaming);
	getData("is_cb_block",0,m_dbgetSub,the_subs.isCBblock);
	getData("imsiType",0,m_dbgetSub,the_subs.imsitype);
	getData("imsi",0,m_dbgetSub,the_subs.imsi);
	getData("act",0,m_dbgetSub,the_subs.act);
	getData("mnrf",0,m_dbgetSub,the_subs.mnrf);
	getData("msc",0,m_dbgetSub,the_subs.msc);
	getData("vlr",0,m_dbgetSub,the_subs.vlr);
	m_datUpdate->setVlr(the_subs.vlr);
        m_datUpdate->setIMSI(the_subs.imsi);
        m_datUpdate->setMsc(the_subs.msc);
	if(the_subs.imsi.empty())
	{
		loginfo(m_strCRF, "BSGmsc::checkSubscriber imsi is empty  Exit");
		return ret;
	}
	if(the_subs.act != 1)
	{
		errmsg = "Subscriber blocked. MSISDN:"+ the_subs.msisdn;
		loginfo(m_strCRF, "BSGmsc::checkSubscriber Subscriber bloked  Exit");
		return (ret = SubscriberStatus::SUBS_STATUS_BLOCKED); 
	}
	if(the_subs.mnrf !=0)
	{
		errmsg ="Subscriber not active. MSISDN:"+ the_subs.msisdn;
		loginfo(m_strCRF, "BSGmsc::checkSubscriber mSubscriber not active  Exit");
	  	return (ret = SubscriberStatus::SUBS_STATUS_NOTACTIVE);
	}
	if(the_subs.vlr.empty() || the_subs.msc.empty())
	{
		errmsg ="Subscriber is purged. MSISDN:"+ the_subs.msisdn;
		loginfo(m_strCRF, "BSGmsc::checkSubscriber Subscriber is purged  Exit");
		return (ret = SubscriberStatus::SUBS_STATUS_PURGED);
	}
	errmsg = "Subscriber found. IMSI =" + the_subs.imsi ;
	loginfo(m_strCRF, "BSGmsc::checkSubscriber Exit");
	return ret = SubscriberStatus::SUBS_STATUS_ACTIVE;
}

retCode BSGmsc::checkNonSubscriberNB(CA::DBInterface* dbInt, CA::DBInterface* dbMnpInt, std::string &nb)
{
	logdebug(m_strCRF, "BSGmsc::checkNonSubscriberNB called");
	retCode ret = retCode::BS_ERROR_UNKOWN ;
	std::string spName(""), spValue(""), mnp_number("");
	int is_onward, is_ported;
	spName = "gmsc_check_nonsubscriber_number";
	spValue = " '"+nb+"'";
	if(!dbInt)
	{
        logerror(m_strCRF,"BSGmsc::checkNonSubscriberNB() DB Connection failure");
        return ret;
    }
	CA::DBMAP dbNonSub;
	logdebug(m_strCRF,"DB gmsc_check_nonsubscriber_number Query = %s %s", spName.c_str(), spValue.c_str());
	ret = executeQuery(dbInt,spName,spValue,dbNonSub);
    if(ret != retCode::BS_SUCCESS)
	{
		logerror(m_strCRF, "Error on gmsc_check_nonsubscriber_number");
    }
    if(dbNonSub.size() > 0)
	{
		nonsubs_type = NonSubsType::NONSUBS_STANDARD;
		return ret;
    }
	// it could be mobile number ported(MNP)
	mnp_number = XlatMNP(dbMnpInt, nb, is_onward, is_ported);
	if(is_ported == 1)
	{
		if(is_onward == 1)
		{
			nonsubs_type = NonSubsType::NONSUBS_ONWARD;
			nb = mnp_number.c_str();
			loginfo(m_strCRF, "BSGmsc::checkNonSubscriberNB NONSUBS_ONWARD Exit");
			return retCode::BS_SUCCESS;
		}
		else
		{
			loginfo(m_strCRF, "BSGmsc::checkNonSubscriberNB Moblie Numer Proted Out");
			ret = retCode::BS_PORTEDOUT_NUMBER;
		}
	}
	else
	{
		ret = retCode::BS_UNALLOCATED_NUMBER;
	}
	return ret;
}

retCode BSGmsc::UnLoad(BSParam *bsp, std::time_t connecttime, std::time_t disconnecttime, std::time_t starttime)
{
	logdebug(m_strCRF, "BSGmsc::UnLoad called");
        #if 0
	 if(m_rsInterface)
             m_rsInterface->UnLoad(bsp,connecttime,disconnecttime,starttime);
        #endif
	retCode ret = retCode::BS_SUCCESS;
	int talktime  = 0;
	if(connecttime <= 0 || disconnecttime <= 0)
	{
		talktime = 0;
	}
	else
	{
		talktime = difftime(disconnecttime,connecttime);
	}
	if(talktime <= 0)
	{
		m_datUpdate->setConnectionState(0);
		talktime  = 0;
	}
	else
	{
	m_datUpdate->setConnectionState(1);
	}
	int appdur = difftime(time(0),starttime);
	m_datUpdate->setAppDur(appdur);
	m_datUpdate->setLegAConnect(talktime);
        m_datUpdate->setLegBConnect(talktime); 
	m_datUpdate->setDisconnectReqBy(1);
        postToDAT();	
	loginfo(m_strCRF, "BSGmsc::UnLoad exit");
	return ret;
}
#if 0
std::string BSGmsc::getTime()
{
	std::time_t t = std::time(0);
    std::tm* now = std::localtime(&t);
	std::ostringstream ss;
	ss << (now->tm_year + 1900);
	ss << "-";
	if(10 > (now->tm_mon + 1))
	{
		ss << "0";
	}
	ss << (now->tm_mon + 1);
	ss << "-";
	if(10 > now->tm_mday)
	{
		ss << "0";
	}
	ss << now->tm_mday;
	ss << " ";
	ss << now->tm_hour;
	ss << ":";
	ss << now->tm_min;
	ss << ":";
	ss << now->tm_sec;
	return ss.str();
}
#endif

