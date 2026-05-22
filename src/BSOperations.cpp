#include "BSOperations.h"
#include "BusinessServer.h"
#include "LogHandler.h"
#define APP "BSOPERATIONS:"

BSParam::BSParam(BSParam& bsParam){
    setCli(bsParam.getCli());
    setCliAddr(bsParam.getCliAddr());
    setDdi(bsParam.getDdi());
    setDdiAddr(bsParam.getDdiAddr());
    setUid(bsParam.getUid());
}
void BSParam::updateBSParam(BSParam& bsParam){
    setUid(bsParam.getUid()); 
	setCli(bsParam.getCli());
    setCliAddr(bsParam.getCliAddr());
    setDdi(bsParam.getDdi());
    setDdiAddr(bsParam.getDdiAddr());
	setCallDirection(bsParam.getCallDirection());
	setCauseCode(bsParam.getCauseCode());
	setState(bsParam.getState());
	setErrorCode(bsParam.getErrorCode());
	setSwitchCode(bsParam.getSwitchCode());
	setRN(bsParam.getRN());
	setTrunkName(bsParam.getTrunkName());
	setSiteCode(bsParam.getSiteCode());
	setCallAgentIP(bsParam.getCallAgentIP());
	setBSIP(bsParam.getBSIP());
	setInterTrunkGroup(bsParam.getInterTrunkgroup());
	setBSstate(bsParam.getBSstate());
	setIwmscPrefix(bsParam.getIwmscPrefix());
	setMvnoPrefix(bsParam.getMvnoPrefix());
	setHlrUrl(bsParam.getHlrUrl());
	setLastKnownError(bsParam.getLastKnownError());
	setUinfo(bsParam.getUinfo());
        setPinfo(bsParam.getPinfo());
	setRedirectingNumber(bsParam.getRedirectingNumber());
	setUnitsCharged(bsParam.getUnitsCharged());
        setIsCcaasCall(bsParam.getIsCcaasCall());
}

std::string BSParam::parseTelephoneNumber(std::string str){
    if(str.empty())
    return str;
    const char* s =  str.c_str();
    char c;
    if(*s ==  '+'){
        s++;
    }
    std::string ret = "";
    while((c=(*s++)) != 0 ){
        if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'D') || (c == '*') || (c == '#') || (c >= 'a' && c <= 'd') || (c == 'P') || (c == 'X')){
            ret += c;
        }else if ((c == '-') || (c == '.') || (c == '(') || (c == ')')){
            continue;
        }else if(c == ';'){
            return ret;
        }else if( c == ':' ){
            ret += s;
            return ret;
        }
    }

    return ret;

}

bool BSParam::decodeJson(const char* jsonStr){
     Document doc;
     Value dataVal;
     if(!jsonStr){
        setLastKnownError("Not Valied string");
        return false;
     }

     if(doc.ParseInsitu((char *)jsonStr).HasParseError()) {
        setLastKnownError("Doc Parser failed");
         return false;
     
     }else if(!doc.IsObject()){
        setLastKnownError("Doc is not object");
        return false;
     }else if (!doc.HasMember("data")){
        setLastKnownError("root ['data'] field not found");
        return false;
     }
     dataVal = doc["data"];


    Value::ConstMemberIterator itr = dataVal.MemberBegin();
    Value::ConstMemberIterator itrEnd = dataVal.MemberEnd();
    if(itr == itrEnd){
        setLastKnownError("no Member found in ['data']");
        return false;
    }

    for(itr; itr != itrEnd; itr++){
        if(!strcmp(itr->name.GetString(), "state")){
            setBSstate(itr->value.GetInt());            
        }else if(!strcmp(itr->name.GetString(), "uid")){
            setUid( itr->value.GetString());            
        }else if(!strcmp(itr->name.GetString(), "cli")){
            setCli(itr->value.GetString());
        }else if(!strcmp(itr->name.GetString(), "ddi")){
            setDdi(itr->value.GetString());
        }else if(!strcmp(itr->name.GetString(), "src_ip")){
            setCliAddr(itr->value.GetString());
        }else if(!strcmp(itr->name.GetString(), "dest_ip")){
            setDdiAddr(itr->value.GetString());
        }else if(!strcmp(itr->name.GetString(), "cause_code")){
            setErrorCode(itr->value.GetInt());
        }else if(!strcmp(itr->name.GetString(), "switch_code")){
            setSwitchCode(itr->value.GetInt());
        }else if(!strcmp(itr->name.GetString(),"ca_ip")){
            setCallAgentIP(itr->value.GetString());
        }else if(!strcmp(itr->name.GetString(),"redirect_no")){
            setRN(itr->value.GetString());
        }else if(!strcmp(itr->name.GetString(), "short_cli")){
            setShortCli(itr->value.GetString());
        }else if(!strcmp(itr->name.GetString(), "is_ccaas")){
            setIsCcaasCall(itr->value.GetInt());
        }

    }
    setCli(parseTelephoneNumber(getCli()));
    //if(strcmp(getDdi().substr(0,3).c_str(),"PBX") != 0)
        setDdi(parseTelephoneNumber(getDdi()));
    setBSIP(GetLocalIPAddress(AF_INET));
    setLastKnownError("Doc Parsed Succefully");
    return true;
}
void BSParam::setBSStateFromCallState(BSCallState State){
    switch(State){
        case BSCallState::STATE_DIALING:
        case BSCallState::STATE_ALERTING:
        case BSCallState::STATE_CONNECTED:
        case BSCallState::STATE_DISCONNECTED:
        case BSCallState::STATE_PROCEEDING:
        setBSstate((int)EN_CA_BUSINESS_STATE::EN_CA_BUSINESS_STATE_PROCESS_SUCCESS);
        break;
        case BSCallState::STATE_TERMINATE:
        setBSstate((int)EN_CA_BUSINESS_STATE::EN_CA_BUSINESS_STATE_PROCESS_FAILED);
        break;
        
        
        
    }
}
std::string BSParam::encodeJson(){
    Document doc;
   StringBuffer strBuf;
   setBSStateFromCallState(getState());
   Writer<StringBuffer> writer(strBuf);
   Writer<StringBuffer> writer1(strBuf);
   Value msgInfoVal(kObjectType);
   Document::AllocatorType& allocator = doc.GetAllocator();
   msgInfoVal.AddMember("uid",setJsonParam(getUid(),doc),allocator);
   msgInfoVal.AddMember("cli",setJsonParam(getCli(),doc),allocator);
   msgInfoVal.AddMember("ddi",setJsonParam(getDdi(),doc),allocator);
   msgInfoVal.AddMember("src_ip",setJsonParam(getCliAddr(),doc),allocator);
   msgInfoVal.AddMember("dest_ip",setJsonParam(getDdiAddr(),doc),allocator);
   msgInfoVal.AddMember("state",setJsonParam((int)getBSstate(),doc),allocator);
   msgInfoVal.AddMember("cause_code",setJsonParam(getErrorCode(),doc),allocator);
   msgInfoVal.AddMember("display_cli",setJsonParam(getCli(),doc),allocator);
   msgInfoVal.AddMember("mod_ddi",setJsonParam(getDdi(),doc),allocator);
   msgInfoVal.AddMember("prefix",setJsonParam("",doc),allocator);
   msgInfoVal.AddMember("route",setJsonParam(getDdiAddr(),doc),allocator);
   msgInfoVal.AddMember("sip_event",setJsonParam((int)getErrorCode(),doc),allocator);
   msgInfoVal.AddMember("brs_ip",setJsonParam(getBSIP(),doc),allocator);
   msgInfoVal.AddMember("ca_ip",setJsonParam(getCallAgentIP(),doc),allocator);
   msgInfoVal.AddMember("user_info",setJsonParam(getUinfo(),doc),allocator);
   msgInfoVal.AddMember("private_info",setJsonParam(getPinfo(),doc),allocator);
   msgInfoVal.AddMember("redirect_no",setJsonParam(getRedirectingNumber(),doc),allocator);
   msgInfoVal.AddMember("is_ccaas",setJsonParam(getIsCcaasCall(),doc),allocator);
   msgInfoVal.AddMember("call_cost",setJsonParam(std::to_string(getUnitsCharged()),doc),allocator);
   msgInfoVal.Accept(writer);
   std::string jsonBuff("");
   int len, msgLen;
   len = strBuf.GetLength();
    if( len > 999) {
        msgLen = len+1;
    } else {
        msgLen = len;
    }
    strBuf.Clear();
    Value dataInfoVal(kObjectType);
    dataInfoVal.AddMember("msg_len", setJsonParam(msgLen+23, doc), allocator);
    dataInfoVal.AddMember("data", msgInfoVal, allocator);
    dataInfoVal.Accept(writer1);
    jsonBuff = strBuf.GetString();
    std::string delimiter("\r\n\r\n");
    jsonBuff.append(delimiter.c_str(), delimiter.length());
    return jsonBuff;
}

/* Method Name: setJsonParam
 ** Description: To set string in document
 ** Parameters: STR strValue,Document &doc
 ** return: Value
 **/
Value BSParam::setJsonParam(STR strValue, Document &doc)
{
   Value retVal;
   retVal.SetString(strValue.c_str(), strValue.length(), doc.GetAllocator());
   return retVal;
}

/* Method Name: setJsonParam
 ** Description: To set integer in document
 ** Parameters: unsigned int iValue, Document &doc
 ** return: Value
 **/
Value BSParam::setJsonParam(unsigned int iValue, Document &doc)
{
   Value retVal;
   retVal = iValue;
   return retVal;
}

BSOperation::BSOperation(std::string crID){
    m_uuid =  crID;
    setTaskId(0);
}

std::string  BSOperation::getTrunkNameWithDdiPrefix(BusinessConfig* pConf){
    logdebug(getInParam()->getUid(),"BSOperation::getTrunkNameWithDdiPrefix() CLI %s DDI %s",getInParam()->getCli().c_str(),getInParam()->getDdi().c_str());
    std::string trunkName="";
    if(pConf){
        std::list<prefix*>* ddiPrefix =  pConf->getPrefixList();
        if(ddiPrefix){
            for(prefix* prf : *ddiPrefix ){
                if(prf){
                    if(std::regex_match (getInParam()->getDdi().c_str(),std::regex(prf->getddiPrefix().c_str()))){
                     trunkName = prf->gettrunkName();
                     setLastKnownError("TrunkName Found based on DDI Prefix "+trunkName);
                     logdebug(getInParam()->getUid(),"BSOperation::getTrunkNameWithDdiPrefix() TrunckName found based on DDi prefixCLI %s DDI %s",getInParam()->getCli().c_str(),getInParam()->getDdi().c_str());
    
                     break;
                    }
                }else{setLastKnownError("DDI prefix object is null");
                logdebug(getInParam()->getUid(),"BSOperation::getTrunkNameWithDdiPrefix() DDI prefix object is null CLI %s DDI %s",getInParam()->getCli().c_str(),getInParam()->getDdi().c_str());}
            }
        }else{setLastKnownError("prefix List is NULL");
        logdebug(getInParam()->getUid(),"BSOperation::getTrunkNameWithDdiPrefix() prefix list is null CLI %s DDI %s",getInParam()->getCli().c_str(),getInParam()->getDdi().c_str());
        }
    }else{setLastKnownError("configuration object is null- getTrunkNameWithDdiPrefix");
    logdebug(getInParam()->getUid(),"BSOperation::getTrunkNameWithDdiPrefix() configuration object is NULL CLI %s DDI %s",getInParam()->getCli().c_str(),getInParam()->getDdi().c_str());
    }
    return trunkName;
}
bool BSOperation::getTrunkProfile(BusinessConfig* pconf){
    logdebug(getInParam()->getUid(),"BSOperation::getTrunkProfile()  CLI %s DDI %s",getInParam()->getCli().c_str(),getInParam()->getDdi().c_str());
    bool ret =  false;
    if(pconf){ 
        CA::CAMap<Trunk*> *trunkList =  pconf->getTrunkList();
        if(trunkList){
            Trunk* trk = trunkList->FindValue(getInParam()->getTrunkName());
            if(trk){
                getInParam()->setSiteCode(trk->getSiteCode());
                getInParam()->setSwitchCode(std::stoi(pconf->getSwitchCode()));
                getInParam()->setIwmscPrefix(trk->getIwmscPrefix());
                getInParam()->setMvnoPrefix(trk->getMvnoPrefix());
                getInParam()->setHlrUrl(trk->getHrlUrl());
                m_strMOMTDBProfile =  trk->getCliDBProfile();
                CA::CAMap<Application*>* appList = pconf->getApplicationList();
                if(appList){
                    Application* app = appList->FindValue(trk->getTaskName());
                    if(app){
                        m_app.setEspDB(app->getEspDB()) ;
                        m_app.setEspParamDB(app->getEspParamDB());
                        m_app.setCcaasDB(app->getCcaasDB());
                        m_app.setRoutingDB(app->getRoutingDB());
                        m_app.setHlrDB(app->getHlrDB());
                        m_app.setMnpDB(app->getMnpDB());
                        m_app.setSwitchDB(app->getSwitchDB());
                        m_app.setTaskName(app->getTaskName());
                        m_app.setAppName(app->getAppName());
                        ret =  true;
                    }else{setLastKnownError("Application configuration not found");
                    logdebug(getInParam()->getUid(),"BSOperation::getTrunkProfile() Application configuration not found CLI %s DDI %s",getInParam()->getCli().c_str(),getInParam()->getDdi().c_str());}
                }else{setLastKnownError("Application list is null");
                 logdebug(getInParam()->getUid(),"BSOperation::getTrunkProfile() Application list is null CLI %s DDI %s",getInParam()->getCli().c_str(),getInParam()->getDdi().c_str());}
            }else {setLastKnownError("Trunk configuration not found");
            logdebug(getInParam()->getUid(),"BSOperation::getTrunkProfile() Trunk configuration not found CLI %s DDI %s",getInParam()->getCli().c_str(),getInParam()->getDdi().c_str());}
         }else{setLastKnownError("Trunk List  is null");
         logdebug(getInParam()->getUid(),"BSOperation::getTrunkProfile()Trunk List  is null CLI %s DDI %s",getInParam()->getCli().c_str(),getInParam()->getDdi().c_str()); }
    }else{setLastKnownError("configuration object is null -getTrunkProfile");
    logdebug(getInParam()->getUid(),"BSOperation::getTrunkProfile() configuration object is null -getTrunkProfile CLI %s DDI %s",getInParam()->getCli().c_str(),getInParam()->getDdi().c_str());}

    return ret;

}
int BSOperation::db_mvno_check_home_subscriber(CA::DBInterface* dbInt,std::string strNumber,std::string strMumber1){
    logdebug(getInParam()->getUid(),"BSOperation::db_mvno_check_home_subscriber() CLI %s DDI %s",getInParam()->getCli().c_str(),getInParam()->getDdi().c_str());
	if(strcmp(strMumber1.substr(0,3).c_str(),"PBX") == 0)
        return 1;

    if(!dbInt){
        updateExecutionStatus(BSCallState::STATE_TERMINATE,OPERATIONEVENT::OPE_TASK_TERMINATE,"DB Interface is NULL", 500);
        return -1;
    }
    if(strNumber.empty()){
        updateExecutionStatus(BSCallState::STATE_TERMINATE,OPERATIONEVENT::OPE_TASK_TERMINATE,"Number is empty", 500);
        return -1;
    }
    
    
    BSInterface *bsInterface = new BSInterface();
    std::string spName = "mvno_check_home_subscriber";
    std::string spValue = "'" + strNumber + "'";
    CA::DBMAP dbmap;
    int nSub = -1;
    retCode ret = bsInterface->executeQuery(dbInt,spName,spValue,dbmap);
    logdebug(getInParam()->getUid(),"BSOperation::%s() DataBase : %s,Query %s",__func__,bsInterface->getDataBaseName(dbInt->m_connectionString).c_str(),dbInt->getLastQuery().c_str()); 
         
    
    if(ret == retCode::BS_SUCCESS){
        if(dbmap.size() > 0){
            bsInterface->getData("is_home_subscriber",0,dbmap,nSub);
            logdebug(getInParam()->getUid(),"BSOperation::db_mvno_check_home_subscriber() is home subscriber CLI %s DDI %s",getInParam()->getCli().c_str(),getInParam()->getDdi().c_str());
            setLastKnownError("Home Subscriber Query executed success Qry:"+dbInt->getLastQuery());
            bsInterface->ClearDBData(dbmap);
            return nSub;
        }
    }
    
    setLastKnownError( dbInt->getLastKnownError()+" Qry: "+dbInt->getLastQuery());
    bsInterface->ClearDBData(dbmap);
    return nSub;
}
bool BSOperation::callInRoute(BusinessConfig* pconf,CA::CAMap<CA::DBInterface*>& dblist){
    logdebug(getInParam()->getUid(),"BSOperation::callInRoute() CLI %s DDI %s",getInParam()->getCli().c_str(),getInParam()->getDdi().c_str());
    
    if(!pconf){ 
        logdebug(getInParam()->getUid(),"BSOperation::callInRoute() configuration failure in callInroute CLI %s DDI %s",getInParam()->getCli().c_str(),getInParam()->getDdi().c_str());
        updateExecutionStatus(BSCallState::STATE_TERMINATE,OPERATIONEVENT::OPE_TASK_TERMINATE,"configuration failure in callInRoute",500);
        return false; 
    }
    std::string trkName =  getTrunkNameWithDdiPrefix(pconf);
    if(trkName.empty()){
        logdebug(getInParam()->getUid(),"BSOperation::callInRoute() DDI prefix not configured CLI %s DDI %s",getInParam()->getCli().c_str(),getInParam()->getDdi().c_str());
        updateExecutionStatus(BSCallState::STATE_TERMINATE,OPERATIONEVENT::OPE_TASK_TERMINATE,"DDi Prefix not configured",404);
        return false;
    }

    getInParam()->setTrunkName(trkName);

    if(getInParam()->getTrace().empty())
    {
        getInParam()->setTrace(getInParam()->getCli()+":"+getInParam()->getDdi()+":"+getInParam()->getTrunkName());
    }

    if(getTrunkProfile(pconf) == false){
        logdebug(getInParam()->getUid(),"BSOperation::callInRoute() Trunck profile Not found CLI %s DDI %s",getInParam()->getCli().c_str(),getInParam()->getDdi().c_str());
        updateExecutionStatus(BSCallState::STATE_TERMINATE,OPERATIONEVENT::OPE_TASK_TERMINATE,"Trunk Profile Not found",500);
        return false;
    }
    int nSub;
    if(strcmp(getInParam()->getDdi().substr(0,3).c_str(),"PBX") == 0 )
    {
        nSub = 1;
    }else{
        CA::DBInterface *dbInterface = dblist.FindValue(m_strMOMTDBProfile);
        if(!dbInterface){
	logdebug(getInParam()->getUid(),"BSOperation::callInRoute() DB connection not found CLI %s DDI %s",getInParam()->getCli().c_str(),getInParam()->getDdi().c_str());
            updateExecutionStatus(BSCallState::STATE_TERMINATE,OPERATIONEVENT::OPE_TASK_TERMINATE,m_strMOMTDBProfile+" - DB Connection not found",500);
            return false;
        }
        

        nSub = db_mvno_check_home_subscriber(dbInterface,getInParam()->getCli(),getInParam()->getDdi());
    }
    switch(nSub){
            case 0:
            {
               logdebug(getInParam()->getUid(),"BSOperation::callInRoute() Spawn GMSC application CLI %s DDI %s",getInParam()->getCli().c_str(),getInParam()->getDdi().c_str());
               updateExecutionStatus(BSCallState::STATE_PROCESSING,OPERATIONEVENT::OPE_GMSC_INIT,"Spawn GMSC Application "+getLastKnownError(),200);
            }
            break;
            case 1:
            {
                logdebug(getInParam()->getUid(),"BSOperation::callInRoute() Spawn Charging Application CLI %s DDI %s",getInParam()->getCli().c_str(),getInParam()->getDdi().c_str());
                updateExecutionStatus(BSCallState::STATE_PROCESSING,OPERATIONEVENT::OPE_CHARING_INIT,"Spawn Charging Application "+getLastKnownError(),200);
            }
            break;
            case -1:
            default:
            {
                logdebug(getInParam()->getUid(),"BSOperation::callInRoute() Default case CLI %s DDI %s",getInParam()->getCli().c_str(),getInParam()->getDdi().c_str());
                updateExecutionStatus(BSCallState::STATE_TERMINATE,OPERATIONEVENT::OPE_TASK_TERMINATE,getLastKnownError(),500);
            }
            break;
        }
    return true;
}
bool BSOperation::callSpawnRoute(BusinessConfig* pconf,CA::CAMap<CA::DBInterface*>& dblist){
    logdebug(getInParam()->getUid(),"BSOperation::callSpawnRoute() CLI %s DDI %s",getInParam()->getCli().c_str(),getInParam()->getDdi().c_str());
    if(!pconf){ 
        setLastKnownError("configuration object is null -callSpawnRoute");
        logdebug(getInParam()->getUid(),"BSOperation::callSpawnRoute() configuration failure in callspawnRoute CLI %s DDI %s",getInParam()->getCli().c_str(),getInParam()->getDdi().c_str());
        updateExecutionStatus(BSCallState::STATE_TERMINATE,OPERATIONEVENT::OPE_TASK_TERMINATE,"configuration failure in CallSpawnRoute",500);
        return false; 
    }
    std::string trkName =  getTrunkNameWithDdiPrefix(pconf);
    if(trkName.empty()){
        logdebug(getInParam()->getUid(),"BSOperation::callSpawnRoute() DDi prefix not configured CLI %s DDI %s",getInParam()->getCli().c_str(),getInParam()->getDdi().c_str());
        updateExecutionStatus(BSCallState::STATE_TERMINATE,OPERATIONEVENT::OPE_TASK_TERMINATE,"DDi Prefix not configured",404);
        return false;
    }
     getInParam()->setTrunkName(trkName);
     if(getTrunkProfile(pconf) == false){
        logdebug(getInParam()->getUid(),"BSOperation::callSpawnRoute() Trunck profile not found CLI %s DDI %s",getInParam()->getCli().c_str(),getInParam()->getDdi().c_str());
        updateExecutionStatus(BSCallState::STATE_TERMINATE,OPERATIONEVENT::OPE_TASK_TERMINATE,"Trunk Profile Not found",500);
        return false;
     }
     CA::DBInterface *dbInterface = dblist.FindValue(m_strMOMTDBProfile);
     if(dbInterface){
        int nSub = db_mvno_check_home_subscriber(dbInterface,getInParam()->getDdi(),getInParam()->getDdi());
        switch(nSub){
            case 1:
            {
                logdebug(getInParam()->getUid(),"BSOperation::callSpawnRoute() Spawn GMSC application CLI %s DDI %s",getInParam()->getCli().c_str(),getInParam()->getDdi().c_str());
               updateExecutionStatus(BSCallState::STATE_PROCESSING,OPERATIONEVENT::OPE_GMSC_INIT,"Spawn GMSC Application",200);
            } 
            break;
            default: // spawn default VMD
            // verfication logic for FWD number required
                spawnDefault(m_app);
            break;
        }
     }else{
        //spawn default could be VMD 
        // verfication logic for FWD number required
        spawnDefault(m_app);
     }
    return true;
}
bool BSOperation::spawnDefault(Application app){
    if(app.getAppName().compare("ESP") ==  0 || app.getAppName().compare("esp") == 0){
        logdebug(getInParam()->getUid(),"BSOperation::spawnDefault() spawn defaul charging appliction CLI %s DDI %s",getInParam()->getCli().c_str(),getInParam()->getDdi().c_str());
        updateExecutionStatus(BSCallState::STATE_PROCESSING,OPERATIONEVENT::OPE_CHARING_INIT,"SpawnDefault Charging Application",200);
    }else if (app.getAppName().compare("GMSC") == 0 || app.getAppName().compare("gmsc") == 0){
        logdebug(getInParam()->getUid(),"BSOperation::spawnDefault() spawn GMSC application CLI %s DDI %s",getInParam()->getCli().c_str(),getInParam()->getDdi().c_str());
        updateExecutionStatus(BSCallState::STATE_PROCESSING,OPERATIONEVENT::OPE_GMSC_INIT,"SpawnDefault GMSC Application",200); 
    }else{
        logdebug(getInParam()->getUid(),"BSOperation::spawnDefault() Default application no found on trunck CLI %s DDI %s",getInParam()->getCli().c_str(),getInParam()->getDdi().c_str());
        updateExecutionStatus(BSCallState::STATE_TERMINATE,OPERATIONEVENT::OPE_TASK_TERMINATE,"Default Application no found on trunk - "+getInParam()->getTrunkName(),500);
    }

    return true;
}
bool BSOperation::callConnectState(BusinessConfig* pconf,CA::CAMap<CA::DBInterface*>& dblist){
    logdebug(getInParam()->getUid(),"BSOperation::callConnectState() CLI %s DDI %s",getInParam()->getCli().c_str(),getInParam()->getDdi().c_str());
    setCallTime(BSCallState::STATE_CONNECTED);
    setCallConnectedStatus(1);
    setTaskId(0);
    setSipErrorCode(200);
    for(int i = 0; i < m_taskList.GetVectorSize();i++ ){
        BSInterface* bsInt = getInterface(i);
        if(!bsInt){
            continue;
        }
        m_taskID =  i;
        getInParam()->setErrorCode(getSipErrorCode());
        switch(bsInt->getAppType()){
            case APPLICATIONTYPE::TYPE_CHARGING:{
                BSCharging* bcCharging =  (BSCharging*)bsInt;
                retCode ret = bcCharging->StartCharging(dblist,getInParam());
            if(ret == retCode::BS_SUCCESS)
                    BusinessServer::setTimerEvent(bcCharging->m_charDt.m_nextChargingTime,(int)BS_TIMER_EVENT::BS_CALL_TIMECHARGING,getUuid(),m_taskID);
            }
            break;
            case APPLICATIONTYPE::TYPE_GMSC:{
                BSGmsc* bsGmsc = (BSGmsc*)bsInt;
            }
            break;
            default:
            break;
        }
    }
    updateExecutionStatus(BSCallState::STATE_CONNECTED,OPERATIONEVENT::OPE_UNKNOWN,"Connect Execute all Task",200);
    return true;
}
bool BSOperation::callDisconnectState(BusinessConfig* pconf,CA::CAMap<CA::DBInterface*>& dblist){
    logdebug(getInParam()->getUid(),"BSOperation::callDisConnectState() CLI %s DDI %s",getInParam()->getCli().c_str(),getInParam()->getDdi().c_str());
    int revert = 0;
    if(getCallConnectedStatus() == 0){
        setCallTime(BSCallState::STATE_CONNECTED);
        revert = 1;
    }
    setCallTime(BSCallState::STATE_DISCONNECTED);
    setTaskId(0);
    for(int i = 0; i < m_taskList.GetVectorSize();i++ ){
        BSInterface* bsInt = getInterface(i);
        if(!bsInt){
            continue;
        }
        m_taskID =  i;
        getInParam()->setErrorCode(getSipErrorCode());
        switch(bsInt->getAppType()){
            case APPLICATIONTYPE::TYPE_CHARGING:{
                BSCharging* bcCharging =  (BSCharging*)bsInt;
                retCode ret = bcCharging->StopCharging(dblist,getInParam(),getConnectedTime(), getDisconnectedTime(),getConnectedTime(), revert);
		bcCharging->ClearMasterDBData();
                delete bcCharging; bcCharging = nullptr;
            }
            break;
            case APPLICATIONTYPE::TYPE_GMSC:{
                BSGmsc* bsGmsc = (BSGmsc*)bsInt;
                retCode ret = bsGmsc->UnLoad(getInParam(),getConnectedTime(),getDisconnectedTime(),getConnectedTime());
                delete bsGmsc; bsGmsc = nullptr;
            }
            break;
            default:
            break;
        }
    }

    switch(getSipErrorCode()){
        case 408: //time out
        {
            logdebug(getInParam()->getUid(),"BSOperation::callDisConnectState() Timeout disconnection executed for all task CLI %s DDI %s",getInParam()->getCli().c_str(),getInParam()->getDdi().c_str());
            updateExecutionStatus(BSCallState::STATE_TERMINATE,OPERATIONEVENT::OPE_TASK_CLEAR,"Timeout disconnection Executed for all task",408);
        }
        break;
        case 487:
        {
            logdebug(getInParam()->getUid(),"BSOperation::callDisConnectState() Cancel request executed for all task  CLI %s DDI %s",getInParam()->getCli().c_str(),getInParam()->getDdi().c_str());
            updateExecutionStatus(BSCallState::STATE_TERMINATE,OPERATIONEVENT::OPE_TASK_CLEAR,"Cancel Request Executed for all task",487);
        }
        break;
        case 404:
        {
            logdebug(getInParam()->getUid(),"BSOperation::callDisConnectState() Cancel request executed for all task  CLI %s DDI %s",getInParam()->getCli().c_str(),getInParam()->getDdi().c_str());
	    if(getInParam()->getState() == BSCallState::STATE_ROUTE_ERROR){
		    getInParam()->setBSstate((int)EN_CA_BUSINESS_STATE::EN_CA_BUSINESS_STATE_PROCESS_FAILED);
		    updateExecutionStatus(BSCallState::STATE_ROUTE_ERROR,OPERATIONEVENT::OPE_TASK_CLEAR,"No routes available",500);
	    }else{
		    updateExecutionStatus(BSCallState::STATE_TERMINATE,OPERATIONEVENT::OPE_TASK_CLEAR,"Cancel Request Executed for all task",500);
	    }
        }
        break;
        case 402:
        {
            logdebug(getInParam()->getUid(),"BSOperation::callDisConnectState() Cancel request executed for all task  CLI %s DDI %s",getInParam()->getCli().c_str(),getInParam()->getDdi().c_str());
            updateExecutionStatus(BSCallState::STATE_TERMINATE,OPERATIONEVENT::OPE_TASK_CLEAR,"Disconnection Executed for all Task",402);
        }
        break;
        default:
        {
            logdebug(getInParam()->getUid(),"BSOperation::callDisConnectState() Disconnection executed for all task -default  CLI %s DDI %s",getInParam()->getCli().c_str(),getInParam()->getDdi().c_str());
            updateExecutionStatus(BSCallState::STATE_DISCONNECTED,OPERATIONEVENT::OPE_TASK_CLEAR,"Disconnection Executed for all Task",getSipErrorCode());
        }
        break;
    }
        
    
        
    
    return true;
}
bool BSOperation::callHttpUpdate(BusinessConfig* pconf,CA::CAMap<CA::DBInterface*>& dblist){
    logdebug(getInParam()->getUid(),"BSOperation::callHttpUpdate() CLI %s DDI %s",getInParam()->getCli().c_str(),getInParam()->getDdi().c_str());
    if(m_taskList.GetVectorSize() > 0){
        int lastTaskId = (m_taskList.GetVectorSize() -1);
        BSInterface* bsInt =  getInterface(lastTaskId);
        if(bsInt){
            switch(bsInt->getAppType()){
                case APPLICATIONTYPE::TYPE_GMSC:{
                    BSGmsc* bsgmsc =  (BSGmsc*)bsInt;
                    retCode ret =  bsgmsc->HLRResponse(dblist,getInParam(),getHttpStatusCode(),getHttpOutput(),m_app);
                    int err =  bsgmsc->getEquivalentSipError(ret);
                     switch (ret)
                     {
                        case retCode::BS_SUCCESS:
                        {
                            logdebug(getInParam()->getUid(),"BSOperation::callHttpUpdate() suspend on dail out CLI %s DDI %s",getInParam()->getCli().c_str(),getInParam()->getDdi().c_str());
                            updateExecutionStatus(BSCallState::STATE_DIALING,OPERATIONEVENT::OPE_BSP_DIAL,"suspend on Dial Out",200);
                        }
                        break;
                        default:
                        {
                            logdebug(getInParam()->getUid(),"BSOperation::callHttpUpdate() Error on HTTp Responce CLI %s DDI %s",getInParam()->getCli().c_str(),getInParam()->getDdi().c_str());  
                            updateExecutionStatus(BSCallState::STATE_TERMINATE,OPERATIONEVENT::OPE_TASK_TERMINATE,"Error on HTTP Response",err);
                        }
                        break;
                     }
                }
                break;
                default:{
                    logdebug(getInParam()->getUid(),"BSOperation::callHttpUpdate() Last task is not GMSC App CLI %s DDI %s",getInParam()->getCli().c_str(),getInParam()->getDdi().c_str());
                    updateExecutionStatus(BSCallState::STATE_TERMINATE,OPERATIONEVENT::OPE_TASK_TERMINATE,"Last Task is not GMSC App",500);
                }
                break;
            }
        }else{
            logdebug(getInParam()->getUid(),"BSOperation::callHttpUpdate() Last task is NULL CLI %s DDI %s",getInParam()->getCli().c_str(),getInParam()->getDdi().c_str());
            updateExecutionStatus(BSCallState::STATE_TERMINATE,OPERATIONEVENT::OPE_TASK_TERMINATE,"Last Task is NULL",500);
        }
    }else{
        logdebug(getInParam()->getUid(),"BSOperation::callHttpUpdate() No Task list found CLI %s DDI %s",getInParam()->getCli().c_str(),getInParam()->getDdi().c_str());
        updateExecutionStatus(BSCallState::STATE_TERMINATE,OPERATIONEVENT::OPE_TASK_TERMINATE,"No Task List found",500);
    }
    return true;
    
}
bool BSOperation::callChargingCycleUpdate(BusinessConfig* pConf,CA::CAMap<CA::DBInterface*>&dblist){
    logdebug(getInParam()->getUid(),"BSOperation::callChargingCycleUpdate() CLI %s DDI %s",getInParam()->getCli().c_str(),getInParam()->getDdi().c_str());
    BSInterface* bsInt =  getInterface(m_taskID);
    if(bsInt){
        switch(bsInt->getAppType()){
            case APPLICATIONTYPE::TYPE_CHARGING:{
                BSCharging* bcCharging =  (BSCharging*)bsInt;
                retCode ret = bcCharging->CharingCycle(dblist,getInParam());
                int err = bcCharging->getEquivalentSipError(ret);
                switch(ret){
                    case retCode::BS_TIME_EXCEED:
                    {
                        logdebug(getInParam()->getUid(),"BSOperation::callChargingCycleUpdate() Call duration exceeds MAX time CLI %s DDI %s",getInParam()->getCli().c_str(),getInParam()->getDdi().c_str());
                        updateExecutionStatus(BSCallState::STATE_TERMINATE,OPERATIONEVENT::OPE_TASK_TERMINATE,"Call Duration exceeds MAX time",err);
                    }
                    break;
                    case retCode::BS_NO_BUNDLE_BALANCE:
                    case retCode::BS_NO_MASTER_BALANCE:
                    {
                        logdebug(getInParam()->getUid(),"BSOperation::callChargingCycleUpdate() No Balance to continue CLI %s DDI %s",getInParam()->getCli().c_str(),getInParam()->getDdi().c_str());
                        updateExecutionStatus(BSCallState::STATE_TERMINATE,OPERATIONEVENT::OPE_TASK_TERMINATE,"No Balance to Continue..",err);
                    }
                    break;
                    case retCode::BS_SUCCESS:
                    {
                        logdebug(getInParam()->getUid(),"BSOperation::callChargingCycleUpdate() charging unit updated CLI %s DDI %s",getInParam()->getCli().c_str(),getInParam()->getDdi().c_str());
                        updateExecutionStatus(BSCallState::STATE_CONNECTED ,OPERATIONEVENT::OPE_TASK_CONNECTED ,"Charging unit updated",err);
                        BusinessServer::setTimerEvent(bcCharging->m_charDt.m_nextChargingTime,(int)BS_TIMER_EVENT::BS_CALL_TIMECHARGING,getUuid(),m_taskID);
                    }
                    break;
                    default:
                    break;
                }
            }
            break;
            default:
            {
                logdebug(getInParam()->getUid(),"BSOperation::callChargingCycleUpdate() Found interface is not charging App CLI %s DDI %s",getInParam()->getCli().c_str(),getInParam()->getDdi().c_str());
                updateExecutionStatus(BSCallState::STATE_TERMINATE,OPERATIONEVENT::OPE_TASK_TERMINATE,"Found Interface is not charging App",500);
            }
            break;
        }

    }else{ updateExecutionStatus(BSCallState::STATE_TERMINATE,OPERATIONEVENT::OPE_TASK_TERMINATE,"No Interface found for charging",500);
    logdebug(getInParam()->getUid(),"BSOperation::callChargingCycleUpdate() No interface found for charging CLI %s DDI %s",getInParam()->getCli().c_str(),getInParam()->getDdi().c_str());}

    return true;
}
void BSOperation::updateExecutionStatus(BSCallState state, OPERATIONEVENT event, std::string errStr,int sipErrCode){

     if(getInParam()){
        getInParam()->setState(state);
        getInParam()->setErrorCode(sipErrCode);
        setLastKnownError(errStr);
        setSipErrorCode(sipErrCode);
        setOperationEvent(event);
    }else{
        setLastKnownError("Call Param Object is null");
        setSipErrorCode(sipErrCode);
        setOperationEvent(OPERATIONEVENT::OPE_UNKNOWN);
    }
}

bool BSOperation::loadAlternateRoute(CA::CAMap<CA::DBInterface*>& dblist){
    logdebug(getInParam()->getUid(),"BSOperation::loadAlternateRoute() CLI %s DDI %s",getInParam()->getCli().c_str(),getInParam()->getDdi().c_str());
    BSInterface* bsInt =  getInterface(m_taskID);
    if(bsInt){
        switch(bsInt->getAppType()){
            case APPLICATIONTYPE::TYPE_CHARGING:
                {
                    BSCharging* bcCharging =  (BSCharging*)bsInt;
                    retCode ret = bcCharging->FetchAlternateRoute(dblist,getInParam(),m_app);
                    if(ret == retCode::BS_NO_ROUTE)
		    {
                            updateExecutionStatus(BSCallState::STATE_ROUTE_ERROR,OPERATIONEVENT::OPE_TASK_CLEAR,"No routes available",404);
			    return false;
		    }
                    break;
                }
            default:
                break;
        }
    }
    return true;
}

void BSOperation::copyOperation(BSOperation* bsSrc){
    if(bsSrc->getInParam()){
        std::unique_ptr<BSParam> bsp(new BSParam());
        bsp->updateBSParam(*(bsSrc->getInParam()));
        updateInParam(std::move(bsp));
    }

    setLastKnownError(bsSrc->getLastKnownError());
    setUuid(bsSrc->getUuid());
    setSipErrorCode(bsSrc->getSipErrorCode());
    setOperationEvent(bsSrc->getOperationEvent());
    setTaskId(bsSrc->getTaskId());
    setCallConnectedStatus(bsSrc->getCallConnectedStatus());
    setCallTimeOut(bsSrc->getstrCallTimeout());
    setHttpOutput(bsSrc->getHttpOutput());
    setHttpStatusCode(bsSrc->getHttpStatusCode());
    setConnectedTime(bsSrc->getConnectedTime());
    setDisconnectedtime(bsSrc->getDisconnectedTime());
    
    m_app.copy(bsSrc->m_app);
}
bool BSOperation::loadCharing(CA::CAMap<CA::DBInterface*>& dblist){
    logdebug(getInParam()->getUid(),"BSOperation::loadCharing() CLI %s DDI %s",getInParam()->getCli().c_str(),getInParam()->getDdi().c_str());
    BSCharging *chargingInterface = new BSCharging(getInParam(),m_app);
    if(!chargingInterface){
        logdebug(getInParam()->getUid(),"BSOperation::loadCharing()Charging interface creation error CLI %s DDI %s",getInParam()->getCli().c_str(),getInParam()->getDdi().c_str());
        updateExecutionStatus(BSCallState::STATE_TERMINATE,OPERATIONEVENT::OPE_TASK_TERMINATE,"Charging Inerface creation error",500);
        return false;
    }  
    retCode ret = chargingInterface->InitCharingAccount(dblist,getInParam(),m_app);
   // int err =  (getInParam()->getErrorCode() > 0)? getInParam()->getErrorCode() : chargingInterface->getEquivalentSipError(ret);
    int err = chargingInterface->getEquivalentSipError(ret);
    switch(ret){
        case retCode::BS_NO_SIGNALIP:
        {
            logdebug(getInParam()->getUid(),"BSOperation::loadCharing() spwaning new task CLI %s DDI %s",getInParam()->getCli().c_str(),getInParam()->getDdi().c_str());
        updateExecutionStatus(BSCallState::STATE_PROCESSING,OPERATIONEVENT::OPE_TASK_SPAWN,"spawning new task",err);
        }
        break;
        case retCode::BS_SUCCESS:
        {
            logdebug(getInParam()->getUid(),"BSOperation::loadCharing() suspend on dail out state CLI %s DDI %s",getInParam()->getCli().c_str(),getInParam()->getDdi().c_str());
        updateExecutionStatus(BSCallState::STATE_DIALING,OPERATIONEVENT::OPE_BSP_DIAL,"suspend on dial out state",err);
        }
        break; 
        default:
        updateExecutionStatus(BSCallState::STATE_TERMINATE,OPERATIONEVENT::OPE_TASK_TERMINATE,chargingInterface->getLastKnownError(),err);
        break;
    }
    //chargingInterface->updateMonitorService(getInParam(),1);//TODO::Dayana::Commented, beacuse currently we are not using Monitor Service.
    insertInterface((BSInterface*)chargingInterface);
    return true;
}
bool BSOperation::loadGmsc(CA::CAMap<CA::DBInterface*>& dblist){
    logdebug(getInParam()->getUid(),"BSOperation::loadGmsc() CLI %s DDI %s",getInParam()->getCli().c_str(),getInParam()->getDdi().c_str());
    BSGmsc *GmscInterface = new BSGmsc(getInParam(),m_app);
    if(!GmscInterface){
        logdebug(getInParam()->getUid(),"BSOperation::loadGmsc()  GMSC interface creation error CLI %s DDI %s",getInParam()->getCli().c_str(),getInParam()->getDdi().c_str());
        updateExecutionStatus(BSCallState::STATE_TERMINATE, OPERATIONEVENT::OPE_TASK_TERMINATE,"GMSC Interface creation error",500);
        return false;
    }
    retCode ret = GmscInterface->Load(dblist,getInParam(),m_app);
    int err = GmscInterface->getEquivalentSipError(ret);
    switch(ret){
        case retCode::BS_SUCCESS:
        case retCode::BS_SUCCESS_DIRECTDIAL:
        {
            logdebug(getInParam()->getUid(),"BSOperation::loadGmsc()  suspend on dail out CLI %s DDI %s",getInParam()->getCli().c_str(),getInParam()->getDdi().c_str());
        updateExecutionStatus(BSCallState::STATE_DIALING,OPERATIONEVENT::OPE_BSP_DIAL,"suspend on dial out",err);
        }
        break;
        case retCode::BS_NO_SIGNALIP:
        {
            logdebug(getInParam()->getUid(),"BSOperation::loadGmsc() spawning neew task  CLI %s DDI %s",getInParam()->getCli().c_str(),getInParam()->getDdi().c_str());
        updateExecutionStatus(BSCallState::STATE_PROCESSING,OPERATIONEVENT::OPE_TASK_SPAWN,"spawing new task",err);
        }
        break;
        case retCode::BS_HLR_REQUEST:
        {
            setHttpRequest(GmscInterface->getHttpServiceMsg());
            updateExecutionStatus(BSCallState::STATE_PROCESSING,OPERATIONEVENT::OPE_HTTP_REQUEST,"suspend on HTTP Request",err);
            logdebug(getInParam()->getUid(),"BSOperation::loadGmsc() suspend on Http request CLI %s DDI %s",getInParam()->getCli().c_str(),getInParam()->getDdi().c_str());
        }
        break;
        default:
        updateExecutionStatus(BSCallState::STATE_TERMINATE,OPERATIONEVENT::OPE_TASK_TERMINATE,GmscInterface->getLastKnownError(),err);
        break; 
    }
    //GmscInterface->updateMonitorService(getInParam(),1);//TODO::Dayana::Commented, beacuse currently we are not using Monitor Service.
    insertInterface((BSInterface*)GmscInterface);
return true;
}
bool BSOperation::insertInterface(BSInterface* bsInt){
    if(!bsInt){
        setLastKnownError("BSInterface is null insertInterface");
        return false;
    }
    m_taskList.PushBack(bsInt);
    return true;
}
BSInterface* BSOperation::getInterface(int taskId){
    if(taskId > -1 && taskId < m_taskList.GetVectorSize()){
        return m_taskList.GetValueByPosition(taskId);
    }
return nullptr;
}
bool BSOperation::deleteInterfaces(){
    if(m_taskList.GetVectorSize() > 0){
        //m_taskList.RemoveByPosition(taskId);
        m_taskList.RemoveAll();
        return true;
    }
return false;
} 
