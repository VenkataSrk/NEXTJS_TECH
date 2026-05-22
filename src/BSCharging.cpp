#include "BSCharging.h"
#include "BusinessServer.h"
#include "LogHandler.h"


#define APP "ESP:"

#if 0
BSCharging::BSCharging(BSCallConfObject* bsc){
    m_strSiteCode =  bsc->m_siteCode;
    m_strCli = bsc->m_cli;
    m_strDdi = bsc->m_ddi;
    m_strTrunkName = bsc->m_trunkName;
    m_strSwitchCode = bsc->m_switchCode;
    std::string taskName = bsc->m_taskName->GetValueByPosition(bsc->m_currentAppPos);
    SPAWNTASKINFO* sTask =  bsc->m_spawnTaskInfo->FindValue(taskName);
    if(sTask){
        m_strAccountProfile = sTask->espdb;
        m_strNpOprProfile = sTask->npoprdb;
    }

}
#endif
BSCharging::BSCharging(BSParam* bsp ,Application app){
    if(bsp){
        m_strSiteCode =  bsp->getSiteCode();
        m_strCli =  bsp->getCli();
        m_strDdi =  bsp->getDdi();
        m_strTrunkName = bsp->getTrunkName();
        //m_strSwitchCode =  std::to_string(bsp->getSwitchCode());
        m_strSwitchCode =  bsp->getStrSwitchCode();
        m_strAccountProfile =  app.getEspDB();
        m_strCcaasProfile = app.getCcaasDB();
        m_strNpOprProfile =  app.getMnpDB();
        m_strCRF = bsp->getUid();
	m_strTrace = bsp->getTrace();         
        m_shortCli = bsp->getShortCli();
        setAppType(APPLICATIONTYPE::TYPE_CHARGING);
        m_strAppID =  generateAppID();
        setBsRequestTime();
	m_strSharedDelay =  "120";
	setPackByMasRowID(0);
        //setBSResponseTime();
        logdebug(m_strCRF,"BSCharging::BSCharging() CLI %s DDI %s APPID %s",m_strCli.c_str(),m_strDdi.c_str(),m_strAppID.c_str());
             

    }else{
        // throw exception objection creation failure;
    }
}


BSCharging::~BSCharging(){
}
retCode BSCharging::FetchAlternateRoute(CA::CAMap<CA::DBInterface*>& dblist, BSParam* bsp,Application app){

    logdebug(m_strCRF,"BSCharging::FetchAlternateRoute() CLI %s DDI %s",m_strCli.c_str(),m_strDdi.c_str());
    logdebug(m_strCRF,"%sIncoming Call, Trunk:%s CLI:%s DDI:%s", APP, bsp->getTrunkName().c_str(),bsp->getCli().c_str(),bsp->getDdi().c_str());

    if (dblist.GetMapSize()  == 0){
        setLastKnownError("DB Connection list empty");
        logerror(m_strCRF,"BSCharging::FetchAlternateRoute()%s",getLastKnownError().c_str());
        return retCode::BS_ERROR_UNKOWN;
    }
    if(bsp == nullptr){
        setLastKnownError("CallIn param object is NULL");
        logerror(m_strCRF,"BSCharging::FetchAlternateRoute()%s",getLastKnownError().c_str());
        return retCode::BS_NULL_OBJECT;
    }

    retCode ret =  retCode::BS_SUCCESS;
    // Alternate Routing
    if(m_rsInterface)
    ret =  m_rsInterface->GetAlternateRoute(dblist,bsp);
    return ret;
}

retCode BSCharging::InitCharingAccount(CA::CAMap<CA::DBInterface*>& dblist, BSParam* bsp,Application app){
    
    logdebug(m_strCRF,"BSCharging::InitCharingAccount() CLI %s DDI %s",m_strCli.c_str(),m_strDdi.c_str());
    logdebug(m_strCRF,"%sIncoming Call, Trunk:%s CLI:%s DDI:%s", APP, bsp->getTrunkName().c_str(),bsp->getCli().c_str(),bsp->getDdi().c_str());
    
    if (dblist.GetMapSize()  == 0){
        setLastKnownError("DB Connection list empty");
        logerror(m_strCRF,"BSCharging::InitCharingAccount()%s",getLastKnownError().c_str());
        return retCode::BS_ERROR_UNKOWN;
    }
    if(bsp == nullptr){
        setLastKnownError("CallIn param object is NULL");
        logerror(m_strCRF,"BSCharging::InitCharingAccount()%s",getLastKnownError().c_str());
        return retCode::BS_NULL_OBJECT;
    }
    
    logdebug(m_strCRF,"BSCharging::InitCharingAccount() [Main Acc DB Profile] %s",m_strAccountProfile.c_str() );
    retCode ret =  retCode::BS_SUCCESS;
    if(bsp->getDdi().find("PBX") != std::string::npos){
                bsp->setDdi(bsp->getDdi().substr(3,bsp->getDdi().size()-3));
                m_strDdi = bsp->getDdi();
                bsp->setIsPbxCall(1);
                m_isPbx = 1;
        }
    CA::DBInterface* dbInt = dblist.FindValue(m_strAccountProfile); // Loading Main Account Profile
    if ((ret = DB_es6_GetParamDb(dbInt)) !=  retCode::BS_SUCCESS){
        logerror(m_strCRF,"BSCharging::InitCharingAccount()%s",getLastKnownError().c_str());
        return ret;
    }
    if(m_strParamProfile.empty()){
        setLastKnownError("DB Param Empty");
        logerror(m_strCRF,"BSCharging::InitCharingAccount()%s",getLastKnownError().c_str());
        return retCode::BS_ERROR_UNKOWN;
    }
    if((ret = verifyCallInParam())  !=  retCode::BS_SUCCESS){
        logerror(m_strCRF,"BSCharging::InitCharingAccount()%s",getLastKnownError().c_str());
        return ret;
    }
    
    logdebug(m_strCRF,"BSCharging::InitCharingAccount() [Param Acc DB Profile %s",m_strParamProfile.c_str());
    
    CA::DBInterface* dbParamInt = dblist.FindValue(m_strParamProfile);  
    if((ret = DB_es5_get_access(dbParamInt,bsp)) != retCode::BS_SUCCESS){
        logerror(m_strCRF,"BSCharging::InitCharingAccount()%s",getLastKnownError().c_str());
        return ret;
    }
    logdebug(m_strCRF,"BSCharging::InitCharingAccount() [NPOPR Acc DB Profile] %s",m_strNpOprProfile.c_str());
    
    CA::DBInterface* dbnpoprInt = dblist.FindValue(m_strNpOprProfile); 
    if((ret = validateAccount(dbInt,dbParamInt,dbnpoprInt,bsp)) != retCode::BS_SUCCESS){
        logerror(m_strCRF,"BSCharging::InitCharingAccount()%s",getLastKnownError().c_str());
        return ret;
    }

    bsp->setInterTrunkGroup("PIPER");
   // setBSResponseTime();
    // Routing 
    std::unique_ptr<BSRouting> rsInterface (new BSRouting(bsp,app));
    m_rsInterface =  std::move(rsInterface);
    ret =  m_rsInterface->InitiateRouting(dblist,bsp);
    return ret;
}
retCode BSCharging::validateAccount(CA::DBInterface* dbInt,CA::DBInterface* dbParamInt,CA::DBInterface* dbnpoprInt,BSParam* bsp){
    
    logdebug(m_strCRF,"BSCharging::validateAccount() CLI %s DDI %s",m_strCli.c_str(),m_strDdi.c_str());
     retCode ret = retCode::BS_SUCCESS;
     
     
     ret = getAccountDetails(dbInt,bsp);
        switch(ret) {
        case retCode::BS_DB_NO_RECORD:
            setLastKnownError("MVNO Account Not Found");
        break;
        case retCode::BS_SUCCESS:
            ret = getAccountBalance(dbInt,m_access.telco_code,m_accountMvnoV2.custcode,m_accountMvnoV2.batchcode,m_accountMvnoV2.serialcode);

            if (ret == retCode::BS_SUCCESS){
                if((ret = onActiveAccount(dbInt,dbParamInt,dbnpoprInt,bsp)) == retCode::BS_SUCCESS){
                    bsp->setDdi(m_strXlatDdi);
                    
                }
            }
        break;
        default:
        break;
        }
    return ret;
}
retCode BSCharging::onActiveAccount(CA::DBInterface* dbInt,CA::DBInterface* dbParamInt,CA::DBInterface* dbnpoprInt,BSParam* bsp){
    logdebug(m_strCRF,"BSCharging::onActiveAccount() CLI %s DDI %s",m_strCli.c_str(),m_strDdi.c_str());
    retCode ret =  retCode::BS_SUCCESS;    
    ret = DB_esp_get_mvnosubscriber_info(dbInt,m_strCli);
    ret = DB_esp_get_promptpath_byMVNO(dbInt,m_strSiteCode,m_strCli);
    m_strXlatCli = prefixNumber(dbParamInt,(CALL_TYPE)m_access.calltype,m_strTrunkName,m_strCli,1);
    
    logdebug(m_strCRF,"BSCharging::onActiveAccount() Xlated CLI %s", m_strXlatCli.c_str());
    ret = DB_esp_getdest_byMVNO_did(dbInt,m_strCli,m_strXlatCli);
    ret =  DB_es2_get_batch(dbInt,m_access.telco_code,m_accountMvnoV2.custcode,m_accountMvnoV2.batchcode);
    ret = DB_esp_get_cardfamily(dbParamInt,m_accountMvnoV2.card_id);
    ret =  verifyCardFeature(dbParamInt);
    ret = DB_esp_getmvno_notif_config(dbParamInt,m_strSiteCode,m_accountMvnoV2.card_id,1,m_accountByCBS.tariffClass);
    ret = DB_es1_get_tariffclass(dbParamInt,m_access.telco_code,m_strSiteCode,m_accountByCBS.tariffClass);
    ret = DB_esp_get_maintenancecharge2(dbParamInt,m_access.telco_code,m_strSiteCode,m_accountByCBS.tariffClass,m_accountByCBS.balance);
    ret = DB_es1_get_alttariffclass(dbParamInt,m_access.telco_code,m_strSiteCode,m_accountByCBS.tariffClass);
    //getCost factor Need to check missing TODO Yadav
    //ret = DB_esp_get_exchangerate(dbInt,)
    //m_cdrUpdate->setCurrCode(curr_code);
    ret  = DB_account_expiry_charge(dbInt,m_accountMvnoV2.custcode,m_accountMvnoV2.batchcode, m_accountMvnoV2.serialcode);
    ret = DB_esp_get_topup_info(dbInt,m_accountMvnoV2.custcode,m_accountMvnoV2.batchcode, m_accountMvnoV2.serialcode);
    ret = DB_es_get_pack6_info(dbInt,m_accountMvnoV2.custcode,m_accountMvnoV2.batchcode, m_accountMvnoV2.serialcode);
    ret = DB_esp_get_package_bymaster(dbInt,m_access.telco_code,m_accountMvnoV2.custcode,m_accountMvnoV2.batchcode,m_accountMvnoV2.serialcode);
    ret = DB_esp_check_special_number(dbParamInt,m_accountMvnoV2.card_id,m_strDdi,1);
    m_strXlatDdi = prefixNumber(dbParamInt,(CALL_TYPE)m_access.calltype,m_strTrunkName,m_strDdi,1);
    if(m_strXlatDdi.empty()){
        setLastKnownError("xlate error DDI");
         logerror(m_strCRF,"BSCharging::onActiveAccount()%s",getLastKnownError().c_str());
        return retCode::BS_XLAT_ERROR;
    }
    logdebug(m_strCRF,"BSCharging::onActiveAccount() Xlated DDI %s", m_strXlatDdi.c_str());
    
    ret =  DB_MNP_get_info(dbnpoprInt,m_strXlatDdi);
    ret = DB_esp_xlat_bybrand(dbInt,m_mnpDet.mnpNum);
    std::string mnpNumber =  m_mnpDet.mnpNum;
    if(!m_byBrandxlat.prefix_new.empty() && !m_byBrandxlat.prefix_old.empty()){
        mnpNumber =  xlat(m_mnpDet.mnpNum,m_byBrandxlat.prefix_old);
        mnpNumber =  m_byBrandxlat.prefix_new +  mnpNumber;
        m_mnpDet.mnpNum =  mnpNumber;
    }
    ret = DB_es3_get_prefix(dbParamInt,m_strSiteCode,mnpNumber,m_tariffClass.prefix_set);
    if(ret != retCode::BS_SUCCESS){
        setLastKnownError("404 route not found");
        logerror(m_strCRF,"BSCharging::onActiveAccount()%s error Code %d",getLastKnownError().c_str(),ret);
        return retCode::BS_NO_ROUTE;
    }
    ret =  DB_esp_get_tariffcode(dbnpoprInt,mnpNumber);
    ret =  DB_esp_is_prefixgroup_blocked_bycli(dbInt,m_strCli,mnpNumber);
    ret =  DB_es_isprefixblocked(dbParamInt,mnpNumber);
    if(ret == retCode::BS_SUCCESS){
        if(mnpNumber.compare(m_mnpDet.isPrefixCodeBlocked) == 0){
            setLastKnownError("outgoing call barred");
            logerror(m_strCRF,"BSCharging::onActiveAccount()%s",getLastKnownError().c_str());
            return retCode::BS_OUTGOING_BARRED;
        }
    } 
    ret = DB_es1_get_premiumnb(dbParamInt,mnpNumber);
    ret = DB_es2_gettimeclass(dbParamInt,m_strSiteCode,m_accountByCBS.tariffClass);
    bsp->setDestcode(m_mnpDet.destcode);

    ret =  DB_es2_get_tariff(dbParamInt,m_access.telco_code,m_strSiteCode,m_mnpDet.destcode,m_accountByCBS.tariffClass,m_access.charge,m_timeClass.time_prd,m_tariffClass.tclass_prem_routecls,m_getTariffMas);
    //m_cdrUpdate->setTariffClass(m_accountByCBS.tariffClass);
    bsp->setTarrifClass(m_accountByCBS.tariffClass);
    if(ret == retCode::BS_SUCCESS){
        switch(m_getTariffMas.preDest){
            case 5: // free of charging
            break;
            case 7:
            break;
            default: // start intial charging
            if((ret = InitiateCharging( dbInt,dbParamInt)) == retCode::BS_SUCCESS);{
                m_strXlatDdi = m_mnpDet.mnpNum;
            }
            
            break;
        }
    }
    return ret;
}
retCode BSCharging::verifyCardFeature(CA::DBInterface* dbParamInt){
    CA::DBMAP dbcardfeatures;

    logdebug(m_strCRF,"BSCharging::verifyCardFeature() CLI %s DDI %s",m_strCli.c_str(),m_strDdi.c_str());
    retCode  ret = DB_esp_get_cardfeatures(dbParamInt,m_accountMvnoV2.card_id,dbcardfeatures);
    if(ret != retCode::BS_SUCCESS){
        logerror(m_strCRF,"BSCharging::verifyCardFeature() %s",getLastKnownError().c_str());
        ClearDBData(dbcardfeatures);
        return ret;
    }
    std::string cardFeatureList = "";
    for(int rowId = 0; dbcardfeatures.size() > rowId; rowId++){
        getData("featuretype",rowId,dbcardfeatures,m_cardFeature.featuretype);
	    getData("param1",rowId,dbcardfeatures,m_cardFeature.param1);
	    getData("param2",rowId,dbcardfeatures,m_cardFeature.param2);
	    getData("param3",rowId,dbcardfeatures,m_cardFeature.param3);
        cardFeatureList = cardFeatureList + "ftr-" + std::to_string(m_cardFeature.featuretype)+"("+ m_cardFeature.param1+"/"+m_cardFeature.param2+"/"+m_cardFeature.param3+")";
        logdebug(m_strCRF, "BSCharging::verifyCardFeature() featuretype %d param1 %s param2 %s param3 %s",m_cardFeature.featuretype,m_cardFeature.param1.c_str(),m_cardFeature.param2.c_str(),m_cardFeature.param3.c_str() );
        switch(m_cardFeature.featuretype){
            case 12: //charge infront feature setting
            if(m_cardFeature.param1.empty())
                m_strSharedDelay =  "120";
            else
                m_strSharedDelay =  m_cardFeature.param1;

            break;
            default:
            break;
        }
    }
    logdebug(m_strCRF, "BSCharging::verifyCardFeature() Card Feature %s", cardFeatureList.c_str());
    ClearDBData(dbcardfeatures);
    return ret;
}
retCode BSCharging::getAccountDetails(CA::DBInterface* dbInt,BSParam* bsp){
    retCode ret = retCode::BS_SUCCESS;
     logdebug(m_strCRF,"BSCharging::getAccountDetails() CLI %s DDI %s",m_strCli.c_str(),m_strDdi.c_str());
    switch((ACCESS_MODE)m_access.mode){
        case ACCESS_MODE::ACSM_MENU: 
      	break;
		case ACCESS_MODE::ACSM_COMBI:
		break;
		case ACCESS_MODE::ACSM_DID_ANI: 
		break;
		case ACCESS_MODE::ACSM_ACCOUNT:
        {
            if(bsp->getIsPbxCall() == 1)
            {
               // ret = CheckPBXCall(dbInt,bsp);
                
            }
        }
		break;
		case ACCESS_MODE::ACSM_DID: 
		break;
		case ACCESS_MODE::ACSM_PIN: 
		break;
		case ACCESS_MODE::ACSM_SIP: 
		break;
		case ACCESS_MODE::ACSM_SRSIP: 
		break;
		case ACCESS_MODE::ACSM_RES_PIN6_CV: 
                break;
		case ACCESS_MODE::ACSM_RES_LOCAL_CV:	
		break;
		case ACCESS_MODE::ACSM_RES_PIN6:
                break;
		case ACCESS_MODE::ACSM_RES_LOCAL:
                break;
		case ACCESS_MODE::ACSM_RES_ROAMING:
                break;
		case ACCESS_MODE::ACSM_BBD_CLI:
                break;
		case ACCESS_MODE::ACSM_BBD_HO:
                break;
		case ACCESS_MODE::ACSM_RES_ZERO2:
                break;
		case ACCESS_MODE::ACSM_RES_OSID: 
		break;
		case ACCESS_MODE::ACSM_FWD: 
		break;
		case ACCESS_MODE::ACSM_MVNO_CLI_FREEDIAL: 
        case ACCESS_MODE::ACSM_MVNO_CLI:
            checkRedirection(bsp);
            ret = DB_esp_getaccount_mvno_v2(dbInt,m_strCli);
        break;
        case ACCESS_MODE::ACSM_MVNO_DID: 
		break;
		case ACCESS_MODE::ACSM_MVNO_PIN: 
		break;
		case ACCESS_MODE::ACSM_MVNO_IVR: 
		break;
		case ACCESS_MODE::ACSM_BBD_LOGIN: 
		break;
		case ACCESS_MODE::ACSM_SME: 
		break;
		default: 
        setLastKnownError("Unknown Access Type");
        ret = retCode::BS_ERROR_UNKOWN;
		break;
    }

    return ret;
}
retCode BSCharging::getAccountBalance(CA::DBInterface* dbInt,std::string teleCode, std::string custCode, int bachCode, int serialCode){
    logdebug(m_strCRF,"BSCharging::getAccountBalance() CLI %s DDI %s",m_strCli.c_str(),m_strDdi.c_str());
    retCode ret = DB_es2_account_by_cbs(dbInt,teleCode,custCode,bachCode,serialCode,m_accountByCBS);
   
    switch (ret)
    {
    case retCode::BS_SUCCESS:
    if(m_accountByCBS.status != 1){
        setLastKnownError("MVNO Account Blocked");
        ret = retCode::BS_ACCOUNT_BLOCKED;
    }
    if(m_accountByCBS.balance <= 1000){
	    double acc_cnxCount = 1.0;
	    int nof_chrag = 0;
	    const double RECHARGE = -5000.0;
	    const double TopUpAmount = 5000.0;
	    retCode ret = DB_es5_update_balance_infront(dbInt,teleCode,custCode,bachCode,serialCode,
			    RECHARGE,acc_cnxCount,nof_chrag,m_strSwitchCode,m_strCRF);	
	    if (ret != retCode::BS_SUCCESS)
	    {
		    logerror(m_strCRF,"BSCharging::getAccountBalance() %s",getLastKnownError().c_str());
		    return ret;
	    }
	    else
	    {
		    if(m_accountByCBS.balance <= m_getTariffMas.cnxunit || m_accountByCBS.balance <= 0)
		    {
			    m_accountByCBS.balance += TopUpAmount;
			    logdebug(m_strCRF,"BSCharging::getAccountBalance() Auto recharge +5000 applied for CLI %s DDI %s Now current balance is %f",m_strCli.c_str(),m_strDdi.c_str(), m_accountByCBS.balance);
		    }
		    else
		    {
			    m_accountByCBS.balance += TopUpAmount;
			    logdebug(m_strCRF,"BSCharging::getAccountBalance() Auto recharge +5000 applied for CLI %s DDI %s Now current balance is %f",m_strCli.c_str(),m_strDdi.c_str(), m_accountByCBS.balance);
		    }
	    }
    }
    default:
        break;
    }

    return ret;

}
std::string BSCharging::prefixNumber(CA::DBInterface* dbInt,CALL_TYPE callType,std::string trunkName, std::string number, int direction){
    logdebug(m_strCRF,"BSCharging::prefixNumber() CLI %s DDI %s",m_strCli.c_str(),m_strDdi.c_str());
    if(callType == CALL_TYPE::CAT_CALLCRO){
        return number;
    }

    retCode ret = DB_es5_get_xlat(dbInt,m_strSiteCode,number,trunkName,direction,m_strSwitchCode,-1,0);

    if(ret != retCode::BS_SUCCESS){
        logerror(m_strCRF,"BSCharging::prefixNumber() %s",getLastKnownError().c_str());
        return "";
    }
    std::string nprefix = "";

    if(m_xlat.prefix.empty() ||  m_xlat.prefix.find("*") != std::string::npos)
        nprefix =  number;
    else if(number.compare(m_xlat.prefix) == 0)
        nprefix =  "";
    else
        nprefix =  xlat(number,m_xlat.prefix);
    std::string strRet =  m_xlat.xlatpfx + nprefix;
    logdebug(m_strCRF,"BSCharging::prefixNumber() prefix %s", strRet.c_str());
    return (strRet);

}



retCode BSCharging::verifyCallInParam(){
    logdebug(m_strCRF,"BSCharging::verifyCallInParam() CLI %s DDI %s",m_strCli.c_str(),m_strDdi.c_str());
    if(m_strSiteCode.empty()){
        setLastKnownError("Site Code Empty");
        return retCode::BS_ERROR_UNKOWN;
    }
    if(m_strTrunkName.empty()){
        setLastKnownError("Trunk Name Empty");
        return retCode::BS_ERROR_UNKOWN;
    }
    if(m_strCli.empty()){
        setLastKnownError("Caller Identification Empty");
        return retCode::BS_ERROR_UNKOWN;

    }
    if(m_strDdi.empty()){
        setLastKnownError("Dialer Identification Empty");
        return retCode::BS_ERROR_UNKOWN;

    }
    if(m_strSwitchCode.empty()){
        setLastKnownError("Business Server Switch Code Empty");
       return retCode::BS_ERROR_UNKOWN;

    }
    return retCode::BS_SUCCESS;
}
retCode BSCharging::InitiateCharging(CA::DBInterface* dbInt,CA::DBInterface* dbParamInt){
    logdebug(m_strCRF,"BSCharging::InitiateCharging() CLI %s DDI %s",m_strCli.c_str(),m_strDdi.c_str());
    retCode ret =  retCode::BS_SUCCESS;
    ret = findBundlePackage(dbInt,dbParamInt);
    switch(ret){
    case retCode::BS_NO_BUNDLE_BALANCE:
    case retCode::BS_NO_BUNDLE_PACKAGE:
        ret =  findMasterPackage(dbInt,dbParamInt);
    break;
    default:
    break;
    }
    return ret;
   
}

retCode BSCharging::findBundlePackage(CA::DBInterface* dbInt,CA::DBInterface* dbParamInt){
    logdebug(m_strCRF,"BSCharging::findBundlePackage()CLI %s DDI %s",m_strCli.c_str(),m_strDdi.c_str());
     retCode ret =  retCode::BS_NO_BUNDLE_PACKAGE;
    m_charDt.m_packChargedTime = 0; // initialize when package start
    m_packageConnectedTime =  std::time(0); 
     for(int rowid =  getPackByMasRowID(); rowid < m_dbPackByMaster.size(); rowid++ ){
        getPackageByMasterData(rowid,m_packByMaster);
        int mvno_brand_id = 0;
        ret =   DB_esp_get_package_exception(dbInt,m_strSiteCode,mvno_brand_id,m_accountByCBS.tariffClass,m_packByMaster.packageid,m_mnpDet.destcode,m_mnpDet.mnpNum);
        ret =  DB_esp_package_authorize_dest(dbInt,m_access.telco_code,m_accountMvnoV2.custcode,m_accountMvnoV2.batchcode,m_accountMvnoV2.serialcode,\
        m_packByMaster.custcode,m_packByMaster.batchcode,m_packByMaster.serialcode,m_packByMaster.packageid,m_strXlatDdi);
        if(ret != retCode::BS_SUCCESS){
            setLastKnownError("Error return  DB_esp_package_authorize_dest");
        }else{
            switch(m_authDest.action){
                case 1:
                
                return retCode::BS_OUTGOING_BARRED;
                break;
                case 2:
                    continue;
                break;
                case 3:
                
                return retCode::BS_OUTGOING_BARRED;
                break;
                default:
                break;

            }
            setLastKnownError(m_authDest.errmsg);
        }

        ret =  DB_es2_account_by_cbs(dbInt,m_access.telco_code,m_packByMaster.custcode,m_packByMaster.batchcode,m_packByMaster.serialcode,m_accountByCBSBundle);
        ret =  DB_es2_gettimeclass(dbInt,m_strSiteCode,m_accountByCBSBundle.tariffClass);
        ret = DB_es1_get_tariffclass(dbParamInt,m_access.telco_code,m_strSiteCode,m_accountByCBSBundle.tariffClass);
        ret =  DB_es2_get_tariff(dbParamInt,m_access.telco_code,m_strSiteCode,m_mnpDet.destcode,m_accountByCBSBundle.tariffClass,m_access.charge,m_timeClass.time_prd,m_tariffClass.tclass_prem_routecls,m_getTariffBundle);
        if(ret == retCode::BS_SUCCESS){
            switch(m_getTariffBundle.preDest){
                case 5: //freeCall
                    ret =  retCode::BS_SUCCESS;
                break;
                default: // bundle charging
                    ret = bundlePackageCharging(dbInt,dbParamInt);
                    if(ret == retCode::BS_NO_BUNDLE_BALANCE){
                        logerror(m_strCRF,"BSCharging::InitiateCharging() %s",getLastKnownError().c_str());
                        continue;
                    }
                break;
            }
            if(ret ==  retCode::BS_SUCCESS){ // On success the loop exits
                setPackByMasRowID(rowid);
                break;
            }
        }else{
            ret =  retCode::BS_NO_BUNDLE_PACKAGE;
        }
    }
    return ret;
}
retCode BSCharging::findMasterPackage(CA::DBInterface* dbInt,CA::DBInterface* dbParamInt){
    retCode ret =  retCode::BS_SUCCESS;
    m_charDt.m_packChargedTime = 0; // initialize when package start
    m_packageConnectedTime =  std::time(0);
    ret = masterPackageCharging(dbInt,dbParamInt);
    if(ret != retCode::BS_SUCCESS){ // on failure no more package check master is last resort
        logerror(m_strCRF,"BSCharging::InitiateCharging() %s",getLastKnownError().c_str());
        
    }
    return ret;
}
retCode BSCharging::updateBundlePackage(CA::DBInterface* dbInt,CA::DBInterface* dbParamInt){
    retCode ret ;
    if(m_getTariffBundle.preDest == 5) // No Charging;
        return retCode::BS_SUCCESS;
    ret =  DB_es2_account_by_cbs(dbInt,m_access.telco_code,m_packByMaster.custcode,m_packByMaster.batchcode,m_packByMaster.serialcode,m_accountByCBSBundle);
    ret =  bundlePackageCharging(dbInt,dbParamInt);
    if(ret == retCode::BS_NO_BUNDLE_BALANCE)
        ret = InitiateCharging(dbInt,dbParamInt);
    return ret;
}

retCode BSCharging::updateMasterPackage(CA::DBInterface* dbInt,CA::DBInterface* dbParamInt){
    retCode ret;
    if(m_getTariffMas.preDest == 5) // free call
        return retCode::BS_SUCCESS;
    ret = DB_es2_account_by_cbs(dbInt,m_access.telco_code,m_accountMvnoV2.custcode,m_accountMvnoV2.batchcode,m_accountMvnoV2.serialcode,m_accountByCBS);
    ret =  masterPackageCharging(dbInt,dbParamInt);
    return ret;
}


retCode BSCharging::bundlePackageCharging(CA::DBInterface* dbInt,CA::DBInterface* dbParamInt){

    logdebug(m_strCRF,"BSCharging::bundlePackageCharging() CLI %s DDI %s",m_strCli.c_str(),m_strDdi.c_str());

    
    retCode ret = retCode::BS_SUCCESS;
    if((m_accountByCBSBundle.balance <= m_getTariffBundle.cnxunit && m_charDt.m_cnxUnitAdded) || m_accountByCBSBundle.balance == 0 ){
        setLastKnownError("No sufficent balance in bundle to make call");
	//postToCDR(); //Dayana
        return retCode::BS_NO_BUNDLE_BALANCE;
    }
    calculateCallUnit(m_getTariffBundle);
    
    if(m_packByMaster.prefixDest.empty() == false && m_packByMaster.prefixDestMode != 0 && m_packByMaster.prefixDestMode != 1){
        std::string xlatedNum =  m_mnpDet.mnpNum;
        ret = DB_esp_get_package_prefixdest_bydid(dbInt,m_access.telco_code,m_packByMaster.custcode,m_packByMaster.batchcode,m_packByMaster.serialcode,m_packByMaster.packageid,m_packByMaster.prefixDestMode,m_packByMaster.prefixDest,m_strCli,xlatedNum);
        if(ret == retCode::BS_SUCCESS){
            m_strDdi =  xlatedNum + m_mnpDet.mnpNum;
        }
      
    }else{
        std::string xlatedNum = m_mnpDet.mnpNum;
        ret = DB_esp_get_package_prefixdest(dbInt,m_access.telco_code,m_packByMaster.custcode,m_packByMaster.batchcode,m_packByMaster.serialcode,m_strCli,xlatedNum);
        if(ret == retCode::BS_SUCCESS){
            m_strDdi = xlatedNum + m_mnpDet.mnpNum;
        }
    }
    float currentBalance = m_accountByCBSBundle.balance;

    if (m_packByMaster.uselimitMode == 1){
        m_packageLimitv2.updateRequired =  false;
                
        ret = DB_esp_get_packagelimit_v2(dbInt,m_packByMaster.custcode,m_packByMaster.batchcode,m_packByMaster.serialcode,m_packByMaster.uselimitMode,m_packByMaster.uselimitDelay,m_packByMaster.uselimitamount,m_packByMaster.uselimitminutes,m_packByMaster.uselimitCall);
        if(ret == retCode::BS_SUCCESS){
            if(m_packageLimitv2.avail_balance <= m_getTariffBundle.cnxunit || m_packageLimitv2.avail_balance <= 0){
                logerror(m_strCRF,"BSCharging::bundlePackageCharging() No Balance Avaiable");
                return retCode::BS_NO_BUNDLE_BALANCE;
            }
        }
        currentBalance = m_packageLimitv2.avail_balance;
        m_packageLimitv2.updateRequired =  true;
    }

    logdebug(m_strCRF,"BSCharging::bundlePackageCharging() current Balance in bundle = %f",currentBalance);

    if (m_charDt.m_allocUnit > currentBalance){
        m_charDt.m_allocUnit = currentBalance;
    }
     

    double acc_cnxCount = 1.0;
    int nof_chrag = 0;
    if(m_charDt.m_allocUnit > 0){
        if(m_packageLimitv2.updateRequired)
            ret = DB_esp_update_packagelimit_v2(dbInt,m_packByMaster.custcode,m_packByMaster.batchcode,m_packByMaster.serialcode,m_packByMaster.uselimitMode,m_packByMaster.uselimitDelay,m_charDt.m_allocUnit,m_charDt.m_packChargedTime);
        ret =  DB_es5_update_balance_infront(dbInt,m_access.telco_code,m_packByMaster.custcode,m_packByMaster.batchcode,m_packByMaster.serialcode,m_charDt.m_allocUnit,acc_cnxCount,nof_chrag,m_strSwitchCode,m_strCRF);
    }
    m_charDt.m_bundleCycleCount++; // 
    if(ret != retCode::BS_SUCCESS){
        logerror(m_strCRF," BSCharging::bundlePackageCharging() %s",getLastKnownError().c_str());
        return ret;
    }
        setPackageType(packageType::PT_BUN);
    return retCode::BS_SUCCESS;
}
retCode BSCharging::masterPackageCharging(CA::DBInterface* dbInt,CA::DBInterface* dbParamInt){
    logdebug(m_strCRF,"BSCharging::masterPackageCharging() CLI %s DDI %s",m_strCli.c_str(),m_strDdi.c_str());
    
    retCode ret =  retCode::BS_SUCCESS;
    if(1==m_getTariffMas.no_package){
        // no Master Package found
        setLastKnownError("No Master Package found");
        logerror(m_strCRF,"BSCharging::masterPackageCharging() %s",getLastKnownError().c_str());
	//postToCDR(); //Dayana
        return retCode::BS_NO_MASTER_PACKAGE;
    }
int talktime =  atoi(m_strSharedDelay.c_str());
    ret = DB_es1_get_progsampl(dbParamInt,m_access.telco_code,m_strSiteCode,m_accountByCBS.tariffClass,m_mnpDet.destcode,m_access.charge,m_timeClass.time_prd,talktime);
    if(ret != retCode::BS_SUCCESS){
        logerror(m_strCRF,"BSCharging::masterPackageCharging %s",getLastKnownError().c_str());
    }
    if(m_accountByCBS.balance <= m_getTariffMas.cnxunit || m_accountByCBS.balance <= 0){
        setLastKnownError("No Enough Balance in Master Package to make call");
        logerror(m_strCRF, "BSCharging::masterPackageCharging() %s", getLastKnownError().c_str());
        //postToCDR(); //Dayana
        return retCode::BS_NO_MASTER_BALANCE;
    }
    std::string inxlateNum;

    ret = DB_esp_get_package_prefixdest(dbInt,m_access.telco_code,m_accountMvnoV2.custcode,m_accountMvnoV2.batchcode,m_accountMvnoV2.serialcode,m_strXlatCli,m_mnpDet.mnpNum);
    if(ret != retCode::BS_SUCCESS){
        logerror(m_strCRF,"BSCharging::masterPackageCharging %s",getLastKnownError().c_str());
    }

    calculateCallUnit(m_getTariffMas);
    float currentBalance = m_accountByCBS.balance;
    logdebug(m_strCRF,"BSCharging::masterPackageCharging() current Balance in bundle = %f",currentBalance);
    if (m_charDt.m_allocUnit > currentBalance){
        m_charDt.m_allocUnit = currentBalance;
    }
     

    double acc_cnxCount = 1.0;
    int nof_chrag = 0;

    ret =  DB_es5_update_balance_infront(dbInt,m_access.telco_code,m_accountMvnoV2.custcode,m_accountMvnoV2.batchcode,m_accountMvnoV2.serialcode,m_charDt.m_allocUnit,acc_cnxCount,nof_chrag,m_strSwitchCode,m_strCRF);
     
    m_charDt.m_bundleCycleCount++; // 
    if(ret != retCode::BS_SUCCESS){
        logerror(m_strCRF," BSCharging::masterPackageCharging() %s",getLastKnownError().c_str());
        return ret;
    }
    setPackageType(packageType::PT_MAS);
    return retCode::BS_SUCCESS;
}
retCode BSCharging::calculateCallUnit(GetTariff curpack){
     logdebug(m_strCRF,"BSCharging::calculateCallUnit() CLI %s DDI %s",m_strCli.c_str(),m_strDdi.c_str());
     m_charDt.m_allocUnit = 0.0; // every cycle this will be intilized 
    switch(m_charDt.m_bundleCycleCount){
        case 0: // first cycle add the connection unit
        m_charDt.m_allocUnit =  curpack.cnxunit;
        m_charDt.m_cnxUnitAdded =  true;
        break;
        default :
        m_charDt.m_cnxUnitAdded = false;
        break;
    }
    m_charDt.m_sharedDelay =  atoi(m_strSharedDelay.c_str());

    if(m_charDt.m_sharedDelay <= curpack.sampledelay){
       m_charDt.m_allocUnit =  m_charDt.m_allocUnit +  curpack.sampleunit;
    }else if(curpack.sampledelay > 0) { 
        if((m_charDt.m_sharedDelay % (int)curpack.sampledelay) == 0){ 
            m_charDt.m_allocUnit = m_charDt.m_allocUnit + (curpack.sampleunit * (m_charDt.m_sharedDelay/curpack.sampledelay));//InitiateCharging
        }
    }else{
        m_charDt.m_allocUnit =  curpack.sampleunit; // on error case only sample unit 
        setLastKnownError("Package sampledelay error");
        logerror(m_strCRF,"BSCharging::calculateCallUnit() %s",getLastKnownError().c_str());
        return retCode::BS_ERROR_UNKOWN;
    }
        
    m_charDt.m_totalAllocUnit += m_charDt.m_allocUnit;
    return retCode::BS_SUCCESS;
}
retCode BSCharging::CalculateChargingCycle(GetTariff curpack){
    logdebug(m_strCRF,"BSCharging::CalculateChargingCycle() CLI %s DDI %s",m_strCli.c_str(),m_strDdi.c_str());

    
      //BSCDRObj.setCnxUnit(curpack.cnxunit);
      //BSCDRObj.setSampleUnit(curpack.sampleunit);
      if(curpack.sampledelay > 0){
            //if(m_charDt.m_allocUnit > curpack.cnxunit)
            if(m_charDt.m_cnxUnitAdded){ // connection unit is added
                logdebug (m_strCRF,"BSCharging::CalculateChargingCycle(((alloc-cnx)/sampunit)*sampdelay) ((%f-%f)/%f)*%d",m_charDt.m_allocUnit, curpack.cnxunit ,curpack.sampleunit,curpack.sampledelay);
                            
                m_charDt.m_nextChargingTime =  ((m_charDt.m_allocUnit-curpack.cnxunit)/curpack.sampleunit)*curpack.sampledelay;
            }
            else {
                logdebug (m_strCRF,"BSCharging::CalculateChargingCycle((alloc/sampunit)*sampdelay) (%f/%f)*%d",m_charDt.m_allocUnit,curpack.sampleunit,curpack.sampledelay);
                m_charDt.m_nextChargingTime =  (m_charDt.m_allocUnit/curpack.sampleunit)*curpack.sampledelay;
            }
        }else{
            logdebug (m_strCRF,"BSCharging::CalculateChargingCycle(shared_delay) %s",m_strSharedDelay.c_str());
            m_charDt.m_nextChargingTime =  atoi(m_strSharedDelay.c_str());
        }
        if( m_charDt.m_nextChargingTime <= 0)
             m_charDt.m_nextChargingTime = atoi(m_strSharedDelay.c_str());

        
        m_charDt.m_totalCharedTime += m_charDt.m_nextChargingTime;
        
        m_charDt.m_packChargedTime += m_charDt.m_nextChargingTime;
        
    
    
    return retCode::BS_SUCCESS;
}
std::string BSCharging::checkRedirection(BSParam* bsp){
    logdebug(m_strCRF,"BSCharging::checkRedirection()CLI %s DDI %s",m_strCli.c_str(),m_strDdi.c_str());
    if(bsp){
        if(bsp->getRN().empty() == false)
            return bsp->getRN();

        }
        return bsp->getCli();    
}

retCode BSCharging::StartCharging(CA::CAMap<CA::DBInterface*>& dbList,BSParam* bsp){
    logdebug(m_strCRF, "BSCharging::StartCharing() CLI %s DDI %s",m_strCli.c_str(),m_strDdi.c_str());
    //CA::DBInterface* dbInt = dblist.FindValue(m_strAccountProfile);
    switch(getPackageType()){
        case packageType::PT_BUN:
            logdebug(m_strCRF, "BSCharging::StartCharing() packageType::PT_BUN");
            CalculateChargingCycle(m_getTariffBundle);
        break;
        case packageType::PT_MAS:
            logdebug(m_strCRF, "BSCharging::StartCharing() packageType::PT_MAS");
            CalculateChargingCycle(m_getTariffMas);
        break;
    }
    m_packageConnectedTime =  std::time(0);
    return retCode::BS_SUCCESS;
}

retCode BSCharging::StopCharging(CA::CAMap<CA::DBInterface*>& dblist,BSParam* bsp,std::time_t connecttime,std::time_t disconnecttime,std::time_t starttime,int revert){
    logdebug(m_strCRF, "BSCharging::StopCharging() CLI %s DDI %s",m_strCli.c_str(),m_strDdi.c_str());
    if(bsp){
        bsp->setCallConnectedTime(getTimeStrFormated(connecttime));
        bsp->setCallDisconnectedTime(getTimeStrFormated(disconnecttime));
    }
   if(m_rsInterface)
	    m_rsInterface->UnLoad(bsp,connecttime,disconnecttime,starttime);
    logdebug(m_strCRF,"BSCharging::StopCharging() [Ccaas Acc DB Profile %s",m_strCcaasProfile.c_str());
    CA::DBInterface* dbInt = dblist.FindValue(m_strAccountProfile); 
    CA::DBInterface* dbCcaasInt = dblist.FindValue(m_strCcaasProfile); 
    CA::DBInterface* dbParamInt = dblist.FindValue(m_strParamProfile); 
    CA::DBInterface* dbnpoprInt = dblist.FindValue(m_strNpOprProfile); 
    retCode ret = retCode::BS_SUCCESS;
    if(revert){
        logdebug(m_strCRF, "BSCharging::StopCharging() Call not connected  Reverting Charging");
        return revertCharging(dbInt,dbParamInt);
    }
    // find the diffrenc for current package charging and update the DB
    double actualTalkTime = std::difftime(disconnecttime,m_packageConnectedTime); // original talked in current package
    double totalTalktime =  std::difftime(disconnecttime,connecttime);
    logdebug (m_strCRF,"BSCharging::StopCharging() %f Actual Talk time in current Pack",actualTalkTime);
    GetTariff curpack ;
    switch(getPackageType()){
        case packageType::PT_BUN:
        curpack = m_getTariffBundle;
        break;
        case packageType::PT_MAS:
        curpack = m_getTariffMas;
        break;
    }
    logdebug(m_strCRF,"BSCharging::StopCharging Actual Talk time %f Total Talk Time %f SampleDelay %d ",std::ceil(actualTalkTime),totalTalktime,curpack.sampledelay );

    if(curpack.sampledelay > 0 && actualTalkTime > 0  && m_charDt.m_packChargedTime > 0){
        float actualSec =  (float) (actualTalkTime/curpack.sampledelay);
        logdebug(m_strCRF, "BSCharging::StopCharging(%f/%d) Actual Second split based on sample delay %f",actualTalkTime,curpack.sampledelay,actualSec);
        float actualInfrontSec =   (m_charDt.m_packChargedTime/curpack.sampledelay);
	//BSCDRObj.setSampleDelay(curpack.sampledelay);
        logdebug(m_strCRF, "BSCharging::StopCharging(%d/%d) Actual Charged infront Second split based on sample delay %f",m_charDt.m_packChargedTime,curpack.sampledelay,actualInfrontSec);
        float remaingSecond = 0;
        if(actualSec >  0 && actualInfrontSec > 0){
            remaingSecond = (float)(std::ceil(actualSec) - actualInfrontSec);
        logdebug(m_strCRF,"BSCharging::StopCharging(%f-%f) Remaing Second in this Package %f", std::ceil(actualSec),actualInfrontSec,remaingSecond);
        }
        m_charDt.m_allocUnit =  curpack.sampleunit * remaingSecond;
        m_charDt.m_totalAllocUnit =  m_charDt.m_totalAllocUnit + m_charDt.m_allocUnit;
    }else{
        logerror(m_strCRF,"BSCharging::StopCharging() actualTalkTime and sample delay are null ");
        return retCode::BS_ERROR_UNKOWN;
    }
    logdebug(m_strCRF,"BSCharging::StopCharging() Total duration Talked %f Total Unit Charged %f",totalTalktime,m_charDt.m_totalAllocUnit);
    bsp->setUnitsCharged(fabs(m_charDt.m_totalAllocUnit));
    double acc_cnxCount = 1.0;
    int nof_chrag = 0;
     switch(getPackageType()){
        case packageType::PT_BUN:
        logdebug(m_strCRF,"BSCharging::StopCharging() - Package Bundle balance update %f total Talk Time %f",m_charDt.m_allocUnit,totalTalktime );
        if(m_charDt.m_allocUnit != 0){
            if(m_packageLimitv2.updateRequired)
                ret = DB_esp_update_packagelimit_v2(dbInt,m_packByMaster.custcode,m_packByMaster.batchcode,m_packByMaster.serialcode,m_packByMaster.uselimitMode,m_packByMaster.uselimitDelay,m_charDt.m_allocUnit,m_charDt.m_packChargedTime);
            ret =  DB_es5_update_balance_infront(dbInt,m_access.telco_code,m_packByMaster.custcode,m_packByMaster.batchcode,m_packByMaster.serialcode,m_charDt.m_allocUnit,acc_cnxCount,nof_chrag,m_strSwitchCode,m_strCRF);
            if(bsp->getIsCcaasCall())
                ret = DB_es5_update_ccaas_balance_infront(dbCcaasInt,bsp);
         
        }
        break;
        case packageType::PT_MAS:
        logdebug(m_strCRF,"BSCharging::StopCharging() - Package Master balance update %f total Talk Time %f",m_charDt.m_allocUnit,totalTalktime );
        if(m_charDt.m_allocUnit != 0){
            ret =  DB_es5_update_balance_infront(dbInt,m_access.telco_code,m_accountMvnoV2.custcode,m_accountMvnoV2.batchcode,m_accountMvnoV2.serialcode,m_charDt.m_allocUnit,acc_cnxCount,nof_chrag,m_strSwitchCode,m_strCRF);
            if(bsp->getIsCcaasCall()){
               ret = DB_es5_update_ccaas_balance_infront(dbCcaasInt,bsp);
              }
        }
               break;
     }
    
    return ret;
}
retCode BSCharging::revertCharging(CA::DBInterface* dbInt,CA::DBInterface* dbParamInt){
    logdebug(m_strCRF,"BSCharging::revertCharging() CLI %s DDI %s",m_strCli.c_str(),m_strDdi.c_str());
    retCode ret;
    double acc_cnxCount = 1.0;
    int nof_chrag = 0;
    if(m_charDt.m_allocUnit > 0){
        m_charDt.m_allocUnit *= -1;
        switch(getPackageType()){
            case packageType::PT_BUN:
            logdebug(m_strCRF,"BSCharging::revertCharging() Bundle Package reverted %f",m_charDt.m_allocUnit );
            if(m_packageLimitv2.updateRequired)
              ret = DB_esp_update_packagelimit_v2(dbInt,m_packByMaster.custcode,m_packByMaster.batchcode,m_packByMaster.serialcode,m_packByMaster.uselimitMode,m_packByMaster.uselimitDelay,m_charDt.m_allocUnit,m_charDt.m_packChargedTime);
            ret =  DB_es5_update_balance_infront(dbInt,m_access.telco_code,m_packByMaster.custcode,m_packByMaster.batchcode,m_packByMaster.serialcode,m_charDt.m_allocUnit,acc_cnxCount,nof_chrag,m_strSwitchCode,m_strCRF); 
            break;
            case packageType::PT_MAS:
                logdebug(m_strCRF,"BSCharging::revertCharging() Master Package reverted %f",m_charDt.m_allocUnit );
                ret =  DB_es5_update_balance_infront(dbInt,m_access.telco_code,m_accountMvnoV2.custcode,m_accountMvnoV2.batchcode,m_accountMvnoV2.serialcode,m_charDt.m_allocUnit,acc_cnxCount,nof_chrag,m_strSwitchCode,m_strCRF);
                break;

        }
    }
    return ret;
}
retCode BSCharging::CharingCycle(CA::CAMap<CA::DBInterface*>& dblist,BSParam* bsp){
    logdebug(m_strCRF, "BSCharging::CharingCycle() CLI %s DDI %s",m_strCli.c_str(),m_strDdi.c_str());
    // check duration 
    if (m_charDt.m_totalCharedTime >= 3600) // need to get from configuration
        return retCode::BS_TIME_EXCEED;

    CA::DBInterface* dbInt = dblist.FindValue(m_strAccountProfile); 
    CA::DBInterface* dbParamInt = dblist.FindValue(m_strParamProfile); 
    CA::DBInterface* dbnpoprInt = dblist.FindValue(m_strNpOprProfile); 
    
    retCode ret = retCode::BS_SUCCESS;
    switch(getPackageType()){
        case packageType::PT_BUN:
            ret = updateBundlePackage(dbInt,dbParamInt);
                
        break;
        case packageType::PT_MAS:
            ret=updateMasterPackage(dbInt,dbParamInt);
                
        break;
        default:
            ret = retCode::BS_NO_PACKAGE_SET;
        break;
    }
    // due to package jump logic below code need is written seperatley do not merge the below code with above code segment  -- Yadav
    if(ret ==  retCode::BS_SUCCESS ){
        switch(getPackageType()){
            case packageType::PT_BUN:
                ret = CalculateChargingCycle(m_getTariffBundle);
            break;
            case packageType::PT_MAS:
                ret =CalculateChargingCycle(m_getTariffMas);
            break;

        }
    }

    return ret;
}




/*---------------------------------------------------------------------------DB Functions--------------------------------------------------------------------------------------*/

retCode BSCharging::DB_es6_GetParamDb(CA::DBInterface* dbInt){
    logdebug(m_strCRF,"BSCharging::DB_es6_GetParamDb() CLI %s DDI %s",m_strCli.c_str(),m_strDdi.c_str());
    std::string spName =  "es6_GetParamDb";
    std::string spValue = "'" + m_strSiteCode + "','" + m_strDdi + "','" + m_strTrunkName + "','" + m_strSwitchCode + "'";
    CA::DBMAP paramDB;
    retCode ret = retCode::BS_SUCCESS;
    if(!dbInt){
        logerror(m_strCRF,"BSCharging::DB_es6_GetParamDb() DB Connection failure");
        return retCode::BS_DB_CONNECTION_FAILED;
    }
    if((ret = executeQuery(dbInt,spName,spValue,paramDB)) == retCode::BS_SUCCESS){
        ret = getData("name",0,paramDB,m_strParamProfile);
        
    }
    logdebug(m_strCRF,"BSCharging::%s() SiteCode: %s, strDdi: %s, TrunkName: %s, SwitchCode: %s",__func__, m_strSiteCode.c_str(),m_strDdi.c_str(),m_strTrunkName.c_str(),m_strSwitchCode.c_str());

    logdebug(m_strCRF,"BSCharging::%s() DataBase : %s,Query %s",__func__,getDataBaseName(dbInt->m_connectionString).c_str(),dbInt->getLastQuery().c_str());
    ClearDBData(paramDB);
    return ret;
}
retCode BSCharging::DB_es5_get_access(CA::DBInterface* dbInt,BSParam* bsp){
    logdebug(m_strCRF,"BSCharging::DB_es5_get_access() CLI %s DDI %s",m_strCli.c_str(),m_strDdi.c_str());
    std::string spName = "es5_get_access";
    std::string spValue = " '" + m_strSiteCode + "','" + m_strDdi + "','" + m_strTrunkName + "'";
    CA::DBMAP  dbes5access;
    if(!dbInt){
        logerror(m_strCRF,"BSCharging::DB_es5_get_access() DB Connection failure");
        return retCode::BS_DB_CONNECTION_FAILED;
    }
    retCode ret = executeQuery(dbInt,spName,spValue,dbes5access);
    logdebug(m_strCRF,"BSCharging::%s() DataBase : %s,Query %s",__func__,getDataBaseName(dbInt->m_connectionString).c_str(),dbInt->getLastQuery().c_str());
    if(ret != retCode::BS_SUCCESS){
	logdebug(m_strCRF, "%sDB_es5_get_access DB:%s, Query:%s - failed %s", APP, m_strParamProfile.c_str(), dbInt->getLastQuery().c_str(), getLastKnownError().c_str());
        ClearDBData(dbes5access);
        return ret;
    }
    if(dbes5access.size() == 0){
        setLastKnownError("No SQL Record Found");
        return retCode::BS_DB_NO_RECORD;
    }
    getData("calltype",0,dbes5access,m_access.calltype);
   	getData("didtype",0,dbes5access,m_access.didtype);
   	getData("did",0,dbes5access,m_access.did);
	getData("mode",0,dbes5access,m_access.mode);
	getData("accesstype",0,dbes5access,m_access.accessType);
	getData("charge",0,dbes5access,m_access.charge);
	getData("telcocode",0,dbes5access,m_access.telco_code);
	getData("sitecode",0,dbes5access,m_access.siteCode);
    if(bsp->getIsPbxCall() == 1)
    {
        getData("custcode",0,dbes5access,m_accountMvnoV2.custcode);
	    getData("serialcode",0,dbes5access,m_accountMvnoV2.serialcode);
	    getData("batchcode",0,dbes5access,m_accountMvnoV2.batchcode);
    }

    logdebug(m_strCRF,"BSCharging::DB_es5_get_access calltype %d didtype %d did %s mode %d accessType %s charge %d  telco_code %s siteCode %s", m_access.calltype,m_access.didtype,m_access.did.c_str(), \
    m_access.mode,m_access.accessType.c_str(),m_access.charge,m_access.telco_code.c_str(),m_access.siteCode.c_str());
    logdebug(m_strCRF,"BSCharging::%s() DataBase : %s,Query %s",__func__,getDataBaseName(dbInt->m_connectionString).c_str(),dbInt->getLastQuery().c_str());
    ClearDBData(dbes5access);
    return retCode::BS_SUCCESS;
}
retCode BSCharging::DB_esp_getaccount_mvno_v2(CA::DBInterface* dbInt,std::string cli){
    logdebug(m_strCRF,"BSCharging::DB_esp_getaccount_mvno_v2() CLI %s DDI %s",m_strCli.c_str(),m_strDdi.c_str());
    std::string spName = "esp_getaccount_mvno_v2";
    std::string spValue = " '" + cli + "'" ;
    CA::DBMAP dbaccmvnov2;
    if(!dbInt){
        logerror(m_strCRF,"BSCharging::DB_esp_getaccount_mvno_v2() DB Connection failure");
         return retCode::BS_DB_CONNECTION_FAILED;
    }
    retCode ret = executeQuery(dbInt,spName,spValue,dbaccmvnov2);

    logdebug(m_strCRF,"BSCharging::%s() DataBase : %s,Query %s",__func__,getDataBaseName(dbInt->m_connectionString).c_str(),dbInt->getLastQuery().c_str());
    if(ret != retCode::BS_SUCCESS){
	logdebug(m_strCRF, "%sDB_esp_getaccount_mvno_v2 DB:%s, Query:%s - failed %s", APP, m_strAccountProfile.c_str(), dbInt->getLastQuery().c_str(), getLastKnownError().c_str());
        ClearDBData(dbaccmvnov2);
        return ret;
    }

    if(dbaccmvnov2.size() == 0){
        setLastKnownError("No SQL Record Found");
        return retCode::BS_DB_NO_RECORD;
    }

    getData("card_id",0,dbaccmvnov2,m_accountMvnoV2.card_id);
	getData("custcode",0,dbaccmvnov2,m_accountMvnoV2.custcode);
  	getData("batchcode",0,dbaccmvnov2,m_accountMvnoV2.batchcode);
	getData("serialcode",0,dbaccmvnov2,m_accountMvnoV2.serialcode);
  	getData("sellprice",0,dbaccmvnov2,m_accountMvnoV2.sellprice);
	getData("cardused",0,dbaccmvnov2,m_accountMvnoV2.cardused);
  	getData("actdiff",0,dbaccmvnov2,m_accountMvnoV2.actdiff);
	getData("actstatus",0,dbaccmvnov2,m_accountMvnoV2.actstatus);
	getData("factor",0,dbaccmvnov2,m_accountMvnoV2.excrate);
    ClearDBData(dbaccmvnov2);
    return retCode::BS_SUCCESS;


}
     
retCode BSCharging::DB_es2_account_by_cbs(CA::DBInterface* dbInt,std::string teleCode, std::string custCode, int bachCode, int serialCode,AccountByCbs& accountByCBS){
    logdebug(m_strCRF,"BSCharging::DB_es2_account_by_cbs() CLI %s DDI %s",m_strCli.c_str(),m_strDdi.c_str());
    std::string spName = "es2_account_by_cbs";
    std::string spValue = " '" + teleCode + "','" + custCode + "'," + std::to_string(bachCode) + "," + std::to_string(serialCode);
    CA::DBMAP dbaccbycbs;
    if(!dbInt){
        logerror(m_strCRF,"BSCharging::DB_es2_account_by_cbs() DB Connection failure");
         return retCode::BS_DB_CONNECTION_FAILED;
    }
    retCode ret = executeQuery(dbInt,spName,spValue,dbaccbycbs);
    //retCode ret = prepareexecuteQuery(dbInt,spName,spValue,dbaccbycbs);

    logdebug(m_strCRF,"BSCharging::%s() DataBase : %s,Query %s",__func__,getDataBaseName(dbInt->m_connectionString).c_str(),dbInt->getLastQuery().c_str());
    if(ret != retCode::BS_SUCCESS){
	logdebug(m_strCRF, "%sDB_es2_account_by_cbs DB:%s, Query:%s - failed %s", APP, m_strAccountProfile.c_str(), dbInt->getLastQuery().c_str(), getLastKnownError().c_str());
        ClearDBData(dbaccbycbs);
        return ret;
	}
    
    if(dbaccbycbs.size() == 0){
        setLastKnownError("No SQL Record Found");
        return retCode::BS_DB_NO_RECORD;
    }

    getData("balance",0,dbaccbycbs,accountByCBS.balance);
    getData("ani",0,dbaccbycbs,accountByCBS.cardId);
	getData("monthlim",0,dbaccbycbs,accountByCBS.monthlim);
	getData("langcode",0,dbaccbycbs,accountByCBS.langset);
	getData("trffclass",0,dbaccbycbs,accountByCBS.tariffClass);
	getData("firstusg",0,dbaccbycbs,accountByCBS.firstusg);
	getData("enddata",0,dbaccbycbs,accountByCBS.expdat);
	getData("status",0,dbaccbycbs,accountByCBS.status);

    ClearDBData(dbaccbycbs);   
    return retCode::BS_SUCCESS;
}

retCode BSCharging::DB_esp_get_mvnosubscriber_info(CA::DBInterface* dbInt,std::string cli){
    logdebug(m_strCRF,"BSCharging::DB_esp_get_mvnosubscriber_info() CLI %s DDI %s",m_strCli.c_str(),m_strDdi.c_str());
    std::string spName = "esp_get_mvnosubscriber_info";
    std::string spValue = " '" + cli + "'";

    CA::DBMAP dbmvnosubscriber;
    if(!dbInt){
        logerror(m_strCRF,"BSCharging::DB_esp_get_mvnosubscriber_info() DB Connection failure");
         return retCode::BS_DB_CONNECTION_FAILED;
    }
    retCode ret = executeQuery(dbInt,spName,spValue,dbmvnosubscriber);

    logdebug(m_strCRF,"BSCharging::%s() DataBase : %s,Query %s",__func__,getDataBaseName(dbInt->m_connectionString).c_str(),dbInt->getLastQuery().c_str());
    if(ret != retCode::BS_SUCCESS){
		logdebug(m_strCRF, "%sDB_esp_get_mvnosubscriber_info DB:%s, Query:%s - failed %s", m_strAccountProfile.c_str(), APP, dbInt->getLastQuery().c_str(), getLastKnownError().c_str());
         ClearDBData(dbmvnosubscriber);
     	 return ret;
    }
    if(dbmvnosubscriber.size() == 0){
        setLastKnownError("No SQL Record Found");
        return retCode::BS_DB_NO_RECORD;
    }

    getData("vlr",0,dbmvnosubscriber,m_mvnoSubsInfo.vlr);
    getData("msc",0,dbmvnosubscriber,m_mvnoSubsInfo.msc);
    getData("isroaming",0,dbmvnosubscriber,m_mvnoSubsInfo.isRoaming);
    ClearDBData(dbmvnosubscriber);
    return retCode::BS_SUCCESS;

}
retCode BSCharging::DB_esp_get_promptpath_byMVNO(CA::DBInterface* dbInt,std::string sitecode, std::string cli){
    logdebug(m_strCRF,"BSCharging::DB_esp_get_promptpath_byMVNO() CLI %s DDI %s",m_strCli.c_str(),m_strDdi.c_str());
    std::string spName = "esp_get_promptpath_byMVNO";
    std::string spValue = " '" + sitecode + "','" + cli + "'";
    CA::DBMAP dbpromptpath;
    if(!dbInt){
        logerror(m_strCRF,"BSCharging::DB_esp_get_promptpath_byMVNO() DB Connection failure");
         return retCode::BS_DB_CONNECTION_FAILED;
    }
    retCode ret = executeQuery(dbInt,spName,spValue,dbpromptpath);
    logdebug(m_strCRF,"BSCharging::%s() DataBase : %s,Query %s",__func__,getDataBaseName(dbInt->m_connectionString).c_str(),dbInt->getLastQuery().c_str());
    if(ret !=  retCode::BS_SUCCESS){
		   	logdebug(m_strCRF, "%sDB_esp_get_promptpath_byMVNO DB:%s, Query:%s - failed %s", APP, m_strAccountProfile.c_str(), dbInt->getLastQuery().c_str(), getLastKnownError().c_str());
        ClearDBData(dbpromptpath);
        return ret;
	}		
    if(dbpromptpath.size() == 0){
        setLastKnownError("No SQL Record Found");
        return retCode::BS_DB_NO_RECORD;
    }
    ClearDBData(dbpromptpath);
    return ret;
}
retCode BSCharging::DB_es5_get_xlat(CA::DBInterface* dbInt,std::string sitecode,std::string number,std::string trunkcode, int direction, std::string switchcode,int nbtype,int xlatSet){
    logdebug(m_strCRF,"BSCharging::DB_es5_get_xlat() CLI %s DDI %s",m_strCli.c_str(),m_strDdi.c_str());
    std::string spName;
    std::string spValue;
    if(m_isPbx == 1)
    {
        //std::string m_shortCli = "205"; // hardcoded for testing
        spName = "es6_get_xlat";
        spValue = "'" + sitecode + "','" + number + "','" + trunkcode + "'," + std::to_string(direction) + ",'" + switchcode + "'," + std::to_string(nbtype) + "," + std::to_string(m_tariffClass.xlatset) + ",'" + m_access.did + "'," + std::to_string(m_access.charge) + ",'" + m_shortCli + "'";

    }else{
        spName = "es5_get_xlat";
        spValue = "'" + sitecode + "','" + number + "','" + trunkcode + "'," + std::to_string(direction) + ",'" + switchcode + "'," + std::to_string(nbtype) + "," + std::to_string(xlatSet) ;
    
    }
    
    CA::DBMAP dbxlat;
    if(!dbInt){
        logerror(m_strCRF,"BSCharging::DB_es5_get_xlat() DB Connection failure");
         return retCode::BS_DB_CONNECTION_FAILED;
    }
    retCode ret =  executeQuery(dbInt,spName,spValue,dbxlat);

    logdebug(m_strCRF,"BSCharging::%s() DataBase : %s,Query %s",__func__,getDataBaseName(dbInt->m_connectionString).c_str(),dbInt->getLastQuery().c_str());
    if(ret != retCode::BS_SUCCESS)
	{	
     logdebug(m_strCRF, "%sDB_es5_get_xlat DB:%s, Query:%s - failed %s", APP, m_strAccountProfile.c_str(), dbInt->getLastQuery().c_str(), getLastKnownError().c_str());
        ClearDBData(dbxlat);
        return ret;
	}	
    if(dbxlat.size() == 0){
        setLastKnownError("No SQL Record Found");
        return retCode::BS_DB_NO_RECORD;
    }
    getData("xlatpfx",0,dbxlat,m_xlat.xlatpfx);
    getData("prefix",0,dbxlat,m_xlat.prefix);
    getData("xlatnbtype",0,dbxlat,m_xlat.xlatnbtype);
    ClearDBData(dbxlat);
    return ret;
}
retCode BSCharging::DB_mnp_get_call_forward(CA::DBInterface* dbInt,std::string trunkcode,std::string didnum, std::string did_nbtype,std::string xlatednum) {
    logdebug(m_strCRF,"BSCharging::DB_mnp_get_call_forward() CLI %s DDI %s",m_strCli.c_str(),m_strDdi.c_str());
    std::string spName = "mnp_get_call_forward";
    int xlatednumtype = 12;
    std::string spValue  = " '" + trunkcode + "','" + xlatednum + "'," + std::to_string(xlatednumtype) + ",'" + didnum + "'," + did_nbtype;
    CA::DBMAP dbcall_forward;
    if(!dbInt){
        logerror(m_strCRF,"BSCharging::DB_mnp_get_call_forward() DB Connection failure");
         return retCode::BS_DB_CONNECTION_FAILED;
    }
    retCode ret = executeQuery(dbInt,spName,spValue,dbcall_forward);

    logdebug(m_strCRF,"BSCharging::%s() DataBase : %s,Query %s",__func__,getDataBaseName(dbInt->m_connectionString).c_str(),dbInt->getLastQuery().c_str());
    if(ret !=  retCode::BS_SUCCESS){
		   	logdebug(m_strCRF, "%sDB_mnp_get_call_forward DB:%s, Query:%s - failed %s", APP, m_strAccountProfile.c_str(), dbInt->getLastQuery().c_str(), getLastKnownError().c_str());
        ClearDBData(dbcall_forward);
        return ret;
	}
    if(dbcall_forward.size() == 0){
        setLastKnownError("No SQL Record Found");
        return retCode::BS_DB_NO_RECORD;
    }
    ClearDBData(dbcall_forward);
    return ret;    
}
retCode BSCharging::DB_esp_getdest_byMVNO_did(CA::DBInterface* dbInt,std::string number, std::string xlatenum) {
logdebug(m_strCRF,"BSCharging::DB_esp_getdest_byMVNO_did() CLI %s DDI %s",m_strCli.c_str(),m_strDdi.c_str());
std::string spName = "esp_getdest_byMVNO_did";
std::string spValue  = " '" + number + "','" + xlatenum + "'";
CA::DBMAP dbdest_byMVNO_did;
if(!dbInt){
        logerror(m_strCRF,"BSCharging::DB_esp_getdest_byMVNO_did() DB Connection failure");
         return retCode::BS_DB_CONNECTION_FAILED;
    }
    retCode ret = executeQuery(dbInt,spName,spValue,dbdest_byMVNO_did);
    logdebug(m_strCRF,"BSCharging::%s() DataBase : %s,Query %s",__func__,getDataBaseName(dbInt->m_connectionString).c_str(),dbInt->getLastQuery().c_str());
    if(ret !=  retCode::BS_SUCCESS){
   	logdebug(m_strCRF, "%sDB_esp_getdest_byMVNO_did DB:%s, Query:%s - failed %s", APP,m_strAccountProfile.c_str(),  dbInt->getLastQuery().c_str(), getLastKnownError().c_str());
        ClearDBData(dbdest_byMVNO_did);
        return ret;
	}	
    if(dbdest_byMVNO_did.size() == 0){
        setLastKnownError("No SQL Record Found");
        return retCode::BS_DB_NO_RECORD;
    }
    ClearDBData(dbdest_byMVNO_did); 
    return ret;

}
retCode BSCharging::DB_es2_get_batch(CA::DBInterface* dbInt,std::string telco_code, std::string cust_code, 
		int batch_code) {
    logdebug(m_strCRF,"BSCharging::DB_es2_get_batch() CLI %s DDI %s",m_strCli.c_str(),m_strDdi.c_str());
std::string spName = "es2_get_batch";
std::string spValue  = " '" + telco_code + "','" + cust_code + "'," + std::to_string(batch_code);
CA::DBMAP dbbatch;
if(!dbInt){
        logerror(m_strCRF,"BSCharging::DB_es2_get_batch() DB Connection failure");
         return retCode::BS_DB_CONNECTION_FAILED;
    }
    retCode ret = executeQuery(dbInt,spName,spValue,dbbatch);
    logdebug(m_strCRF,"BSCharging::%s() DataBase : %s,Query %s",__func__,getDataBaseName(dbInt->m_connectionString).c_str(),dbInt->getLastQuery().c_str());
    if(ret !=  retCode::BS_SUCCESS){
    logdebug(m_strCRF, "%sDB_es2_get_batch DB:%s, Query:%s - failed %s", APP, m_strAccountProfile.c_str(), dbInt->getLastQuery().c_str(), getLastKnownError().c_str());
    ClearDBData(dbbatch);
        return ret;
	}	
    if(dbbatch.size() == 0){
        setLastKnownError("No SQL Record Found");
        return retCode::BS_DB_NO_RECORD;
    }
    ClearDBData(dbbatch);
    return ret;
}

retCode BSCharging::DB_esp_get_cardfamily(CA::DBInterface* dbParamInt,int card_id) {
    logdebug(m_strCRF,"BSCharging::DB_esp_get_cardfamily() CLI %s DDI %s",m_strCli.c_str(),m_strDdi.c_str());
std::string spName = "esp_get_cardfamily";
std::string spValue;
if(m_access.accessType.compare("W") == 0 && m_isPbx == 1){
     spValue  =  " '" + std::to_string(m_accountByCBS.cardId) + "'";
}else{
 spValue  = " '" + std::to_string(card_id) + "'";
}
CA::DBMAP dbcardfamily;
if(!dbParamInt){
        logerror(m_strCRF,"BSCharging::DB_esp_get_cardfamily() DB Connection failure");
         return retCode::BS_DB_CONNECTION_FAILED;
    }
    retCode ret = executeQuery(dbParamInt,spName,spValue,dbcardfamily);
	logdebug(m_strCRF, "%sDB_esp_get_cardfamily DB:%s, Query:%s", APP, m_strParamProfile.c_str(), dbParamInt->getLastQuery().c_str());
    logdebug(m_strCRF,"BSCharging::%s() DataBase : %s,Query %s",__func__,getDataBaseName(dbParamInt->m_connectionString).c_str(),dbParamInt->getLastQuery().c_str());
    if(ret !=  retCode::BS_SUCCESS){
    logdebug(m_strCRF, "%sDB_esp_get_cardfamily DB:%s, Query:%s - failed %s", APP,  m_strParamProfile.c_str(),dbParamInt->getLastQuery().c_str(), getLastKnownError().c_str());
        ClearDBData(dbcardfamily);
        return ret;
	}	
    if(dbcardfamily.size() == 0){
        setLastKnownError("No SQL Record Found");
        return retCode::BS_DB_NO_RECORD;
    }
    ClearDBData(dbcardfamily);
    return ret;
}

retCode BSCharging::DB_esp_get_cardfeatures(CA::DBInterface* dbInt,int cardId,CA::DBMAP& dbcardfeatures) {
    logdebug(m_strCRF,"BSCharging::DB_esp_get_cardfeatures() CLI %s DDI %s",m_strCli.c_str(),m_strDdi.c_str());
std::string spName = "esp_get_cardfeatures";
std::string spValue;
if(m_access.accessType.compare("W") == 0 && m_isPbx == 1){
     spValue  =  " '" + std::to_string(m_accountByCBS.cardId) + "'";
}else{
 spValue  =  " '" + std::to_string(cardId) + "'";
}
if(!dbInt){
        logerror(m_strCRF,"BSCharging::DB_esp_get_cardfeatures() DB Connection failure");
         return retCode::BS_DB_CONNECTION_FAILED;
    }
    retCode ret = executeQuery(dbInt,spName,spValue,dbcardfeatures);
    logdebug(m_strCRF,"BSCharging::%s() DataBase : %s,Query %s",__func__,getDataBaseName(dbInt->m_connectionString).c_str(),dbInt->getLastQuery().c_str());
    if(ret !=  retCode::BS_SUCCESS){
		logdebug(m_strCRF, "%sDB_esp_get_cardfeatures DB:%s, Query:%s - failed %s", APP, m_strParamProfile.c_str(), dbInt->getLastQuery().c_str(), getLastKnownError().c_str());
        return ret;
	}	
    if(dbcardfeatures.size() == 0){
        setLastKnownError("No SQL Record Found");
        return retCode::BS_DB_NO_RECORD;
    }
    
    return ret;
}

retCode BSCharging::DB_esp_getmvno_notif_config(CA::DBInterface* dbInt,std::string sitecode, int card_id, 
		int mvno_brandid, std::string tariff_class) {
            logdebug(m_strCRF,"BSCharging::DB_esp_getmvno_notif_config() CLI %s DDI %s",m_strCli.c_str(),m_strDdi.c_str());
std::string spName = "esp_getmvno_notif_config";
std::string spValue  =  " '" + sitecode + "'," + std::to_string(card_id) +","+ std::to_string(mvno_brandid) + ",'" + tariff_class + "'";
CA::DBMAP dbmvno_notif_config;
if(!dbInt){
        logerror(m_strCRF,"BSCharging::DB_esp_getmvno_notif_config() DB Connection failure");
         return retCode::BS_DB_CONNECTION_FAILED;
    }
    retCode ret = executeQuery(dbInt,spName,spValue,dbmvno_notif_config);
    logdebug(m_strCRF,"BSCharging::%s() DataBase : %s,Query %s",__func__,getDataBaseName(dbInt->m_connectionString).c_str(),dbInt->getLastQuery().c_str());
    if(ret !=  retCode::BS_SUCCESS){
	   	logdebug(m_strCRF, "%sDB_esp_getmvno_notif_config DB:%s, Query:%s - failed %s", APP, m_strParamProfile.c_str(), dbInt->getLastQuery().c_str(), getLastKnownError().c_str());
        ClearDBData(dbmvno_notif_config);
        return ret;
	}	
    if(dbmvno_notif_config.size() == 0){
        setLastKnownError("No SQL Record Found");
        return retCode::BS_DB_NO_RECORD;
    }
     ClearDBData(dbmvno_notif_config);
    return ret;
}

retCode BSCharging::DB_es1_get_tariffclass(CA::DBInterface* dbParamInt,std::string telco_code, std::string sitecode, std::string tariff_class) {
    logdebug(m_strCRF,"BSCharging::DB_es1_get_tariffclass() CLI %s DDI %s",m_strCli.c_str(),m_strDdi.c_str());
std::string spName = "es1_get_tariffclass";
std::string spValue  = " '" + telco_code  + "','" + sitecode + "','" + tariff_class + "'";
CA::DBMAP dbtariffclass;
if(!dbParamInt){
        logerror(m_strCRF,"BSCharging::DB_es1_get_tariffclass() DB Connection failure");
         return retCode::BS_DB_CONNECTION_FAILED;
    }
    retCode ret = executeQuery(dbParamInt,spName,spValue,dbtariffclass);
    logdebug(m_strCRF,"BSCharging::%s() DataBase : %s,Query %s",__func__,getDataBaseName(dbParamInt->m_connectionString).c_str(),dbParamInt->getLastQuery().c_str());
    if(ret !=  retCode::BS_SUCCESS){
	logdebug(m_strCRF, "%sDB_es1_get_tariffclass DB:%s, Query:%s - failed %s", APP, m_strParamProfile.c_str(), dbParamInt->getLastQuery().c_str(), getLastKnownError().c_str());
        ClearDBData(dbtariffclass);
        return ret;
	}	
    if(dbtariffclass.size() == 0){
        setLastKnownError("No SQL Record Found");
        return retCode::BS_DB_NO_RECORD;
    }
    getData("prefixset",0,dbtariffclass,m_tariffClass.prefix_set);
	getData("prem_routecls",0,dbtariffclass,m_tariffClass.tclass_prem_routecls);
	getData("gradecode",0,dbtariffclass,m_tariffClass.route_grade);
	getData("onegocharge",0,dbtariffclass,m_tariffClass.onego_charge);
    getData("xlatset",0,dbtariffclass,m_tariffClass.xlatset);
    ClearDBData(dbtariffclass);
    return ret;
}

retCode BSCharging::DB_esp_get_maintenancecharge2(CA::DBInterface* dbInt,std::string telco_code, std::string sitecode, 
		std::string tariff_class, double accountbal) {
            logdebug(m_strCRF,"BSCharging::DB_esp_get_maintenancecharge2() CLI %s DDI %s",m_strCli.c_str(),m_strDdi.c_str());
std::string spName = "esp_get_maintenancecharge2";
std::string spValue  =  " '" + telco_code  + "','" + sitecode + "','" + tariff_class + "',"+ std::to_string(accountbal);
CA::DBMAP dbmaintenancecharge2;
if(!dbInt){
        logerror(m_strCRF,"BSCharging::DB_esp_get_maintenancecharge2() DB Connection failure");
         return retCode::BS_DB_CONNECTION_FAILED;
    }
    retCode ret = executeQuery(dbInt,spName,spValue,dbmaintenancecharge2);
    logdebug(m_strCRF,"BSCharging::%s() DataBase : %s,Query %s",__func__,getDataBaseName(dbInt->m_connectionString).c_str(),dbInt->getLastQuery().c_str());
    if(ret !=  retCode::BS_SUCCESS){
 	logdebug(m_strCRF, "%sDB_esp_get_maintenancecharge2 DB:%s, Query:%s - failed %s", APP, m_strParamProfile.c_str(), dbInt->getLastQuery().c_str(), getLastKnownError().c_str());
        ClearDBData(dbmaintenancecharge2);
        return ret;
	}	
    if(dbmaintenancecharge2.size() == 0){
        setLastKnownError("No SQL Record Found");
        return retCode::BS_DB_NO_RECORD;
    }
    ClearDBData(dbmaintenancecharge2);
    return ret;
}


retCode BSCharging::DB_es1_get_alttariffclass(CA::DBInterface* dbInt,std::string telco_code, std::string sitecode, 
		std::string tariff_class) {
            logdebug(m_strCRF,"BSCharging::DB_es1_get_alttariffclass()");
std::string spName = "es1_get_alttariffclass";
std::string spValue  =  " '" + telco_code  + "','" + sitecode + "','" + tariff_class + "'";
CA::DBMAP dbalttariffclass;
if(!dbInt){
        logerror(m_strCRF,"BSCharging::DB_es1_get_alttariffclass() DB Connection failure");
         return retCode::BS_DB_CONNECTION_FAILED;
    }
    retCode ret = executeQuery(dbInt,spName,spValue,dbalttariffclass);
    logdebug(m_strCRF,"BSCharging::%s() DataBase : %s,Query %s",__func__,getDataBaseName(dbInt->m_connectionString).c_str(),dbInt->getLastQuery().c_str());
    if(ret !=  retCode::BS_SUCCESS){
	logdebug(m_strCRF, "%sDB_es1_get_alttariffclass DB:%s, Query:%s - failed %s", APP, m_strParamProfile.c_str(),dbInt->getLastQuery().c_str(), getLastKnownError().c_str());
        ClearDBData(dbalttariffclass);
        return ret;
	}	
    if(dbalttariffclass.size() == 0){
        setLastKnownError("No SQL Record Found");
        return retCode::BS_DB_NO_RECORD;
    }
    ClearDBData(dbalttariffclass);
    return ret;
}


retCode BSCharging::DB_esp_get_exchangerate(CA::DBInterface* dbInt,std::string curr_code, std::string subres_currcode) {
    logdebug(m_strCRF,"BSCharging::DB_esp_get_exchangerate() CLI %s DDI %s",m_strCli.c_str(),m_strDdi.c_str());
std::string spName = "esp_get_exchangerate";
std::string spValue  =  " '" + curr_code + "','" + 
		               subres_currcode +"'";
CA::DBMAP dbexchangerate;
if(!dbInt){
        logerror(m_strCRF,"BSCharging::DB_esp_get_exchangerate() DB Connection failure");
         return retCode::BS_DB_CONNECTION_FAILED;
    }
    retCode ret = executeQuery(dbInt,spName,spValue,dbexchangerate);
    logdebug(m_strCRF,"BSCharging::%s() DataBase : %s,Query %s",__func__,getDataBaseName(dbInt->m_connectionString).c_str(),dbInt->getLastQuery().c_str());
    if(ret !=  retCode::BS_SUCCESS){
    logdebug(m_strCRF, "%sDB_esp_get_exchangerate DB:%s, Query:%s - failed %s", APP, m_strAccountProfile.c_str(), dbInt->getLastQuery().c_str(), getLastKnownError().c_str());
    ClearDBData(dbexchangerate);
        return ret;
	}	
    if(dbexchangerate.size() == 0){
        setLastKnownError("No SQL Record Found");
        return retCode::BS_DB_NO_RECORD;
    }
    ClearDBData(dbexchangerate);
    return ret;
}


retCode BSCharging::DB_account_expiry_charge(CA::DBInterface* dbInt,std::string cust_code, int batch_code, int serial_code) {
    logdebug(m_strCRF,"BSCharging::DB_account_expiry_charge() CLI %s DDI %s",m_strCli.c_str(),m_strDdi.c_str());
std::string spName = "account_expiry_charge";
std::string time = GetTime();
std::string spValue  =   " '" + cust_code + "'," + std::to_string(batch_code) + "," + std::to_string(serial_code) + ",'" + time +"','ESP'";

CA::DBMAP dbaccount_expiry_charge;
if(!dbInt){
        logerror(m_strCRF,"BSCharging::DB_account_expiry_charge() DB Connection failure");
         return retCode::BS_DB_CONNECTION_FAILED;
    }
    retCode ret = executeQuery(dbInt,spName,spValue,dbaccount_expiry_charge);
    logdebug(m_strCRF,"BSCharging::%s() DataBase : %s,Query %s",__func__,getDataBaseName(dbInt->m_connectionString).c_str(),dbInt->getLastQuery().c_str());
    if(ret !=  retCode::BS_SUCCESS){
  	logdebug(m_strCRF, "%sDB_account_expiry_charge DB:%s, Query:%s - failed %s", APP, m_strAccountProfile.c_str(), dbInt->getLastQuery().c_str(), getLastKnownError().c_str());
        ClearDBData(dbaccount_expiry_charge);
        return ret;
	}	
    if(dbaccount_expiry_charge.size() == 0){
        setLastKnownError("No SQL Record Found");
        return retCode::BS_DB_NO_RECORD;
    }
    ClearDBData(dbaccount_expiry_charge);
    return ret;
}

retCode BSCharging::DB_esp_get_topup_info(CA::DBInterface* dbInt,std::string cust_code, int batch_code, int serial_code)  {
    logdebug(m_strCRF,"BSCharging::DB_esp_get_topup_info() CLI %s DDI %s",m_strCli.c_str(),m_strDdi.c_str());
std::string spName = "esp_get_topup_info";
std::string spValue  =   " '" + cust_code + "',"+ std::to_string(batch_code) + "," + std::to_string(serial_code);
CA::DBMAP dbtopup_info;
if(!dbInt){
        logerror(m_strCRF,"BSCharging::DB_esp_get_topup_info() DB Connection failure");
         return retCode::BS_DB_CONNECTION_FAILED;
    }
    retCode ret = executeQuery(dbInt,spName,spValue,dbtopup_info);
    logdebug(m_strCRF,"BSCharging::%s() DataBase : %s,Query %s",__func__,getDataBaseName(dbInt->m_connectionString).c_str(),dbInt->getLastQuery().c_str());
    if(ret !=  retCode::BS_SUCCESS){
	logdebug(m_strCRF, "%sDB_esp_get_topup_info DB:%s, Query:%s - failed %s", APP, m_strAccountProfile.c_str(), dbInt->getLastQuery().c_str(), getLastKnownError().c_str());
        ClearDBData(dbtopup_info);
        return ret;
	}	
    if(dbtopup_info.size() == 0){
        setLastKnownError("No SQL Record Found");
        return retCode::BS_DB_NO_RECORD;
    }
    getData("tp_tariffclass",0,dbtopup_info,m_topupTariff.tp_tariffclass);
	getData("createdate",0,dbtopup_info,m_topupTariff.tp_createdate);
    ClearDBData(dbtopup_info);
    return ret;
}

retCode BSCharging::DB_es_get_pack6_info(CA::DBInterface* dbInt,std::string cust_code, int batch_code, int serial_code) {
    logdebug(m_strCRF,"BSCharging::DB_es_get_pack6_info() CLI %s DDI %s",m_strCli.c_str(),m_strDdi.c_str());
std::string spName = "es_get_pack6_info";
std::string spValue  =  " '" + cust_code +"'," + std::to_string(batch_code) + "," + std::to_string(serial_code);
CA::DBMAP dbpack6_info;
if(!dbInt){
        logerror(m_strCRF,"BSCharging::DB_es_get_pack6_info() DB Connection failure");
         return retCode::BS_DB_CONNECTION_FAILED;
    }
    retCode ret = executeQuery(dbInt,spName,spValue,dbpack6_info);
    logdebug(m_strCRF,"BSCharging::%s() DataBase : %s,Query %s",__func__,getDataBaseName(dbInt->m_connectionString).c_str(),dbInt->getLastQuery().c_str());
    if(ret !=  retCode::BS_SUCCESS){
	logdebug(m_strCRF, "%sDB_es_get_pack6_info DB:%s, Query:%s - failed %s", APP, m_strAccountProfile.c_str(), dbInt->getLastQuery().c_str(), getLastKnownError().c_str());
        ClearDBData(dbpack6_info);
        return ret;
	}	
    if(dbpack6_info.size() == 0){
        setLastKnownError("No SQL Record Found");
        return retCode::BS_DB_NO_RECORD;
    }
    getData("tariffclass",0,dbpack6_info,m_packTariff.tariffClass);
    ClearDBData(dbpack6_info);
    return ret;
}

retCode BSCharging::DB_esp_get_package_bymaster(CA::DBInterface* dbInt,std::string telco_code, std::string cust_code,
	 int batch_code, int serial_code ) {
        logdebug(m_strCRF,"BSCharging::DB_esp_get_package_bymaster() CLI %s DDI %s",m_strCli.c_str(),m_strDdi.c_str());
std::string spName = "esp_get_package_bymaster";
std::string time = GetTime();
std::string spValue  = " '" + telco_code + "','" +cust_code + "'," + std::to_string(batch_code) + "," + std::to_string(serial_code) + ",1,'" +time +"'";
if(!dbInt){
        logerror(m_strCRF,"BSCharging::DB_esp_get_package_bymaster() DB Connection failure");
         return retCode::BS_DB_CONNECTION_FAILED;
    }
    retCode ret = executeQuery(dbInt,spName,spValue,m_dbPackByMaster);
    logdebug(m_strCRF,"BSCharging::%s() DataBase : %s,Query %s",__func__,getDataBaseName(dbInt->m_connectionString).c_str(),dbInt->getLastQuery().c_str());
    if(ret !=  retCode::BS_SUCCESS){
 	logdebug(m_strCRF, "%sDB_esp_get_package_bymaster DB:%s, Query:%s - failed %s", APP, m_strAccountProfile.c_str(), dbInt->getLastQuery().c_str(), getLastKnownError().c_str());
        return ret;
	}	
    if(m_dbPackByMaster.size() == 0){
        setLastKnownError("No SQL Record Found");
        logdebug(m_strCRF, "%sDB_esp_get_package_bymaster DB:%s, Query:%s - failed %s", APP, m_strAccountProfile.c_str(), dbInt->getLastQuery().c_str(), getLastKnownError().c_str());
        return retCode::BS_DB_NO_RECORD;
    }
    ret = getPackageByMasterData(0,m_packByMaster);
  
    return ret;
}

retCode BSCharging::getPackageByMasterData(int rowID,PackageByMaster& packByMaster){
    logdebug(m_strCRF,"BSCharging::getPackageByMasterData() CLI %s DDI %s",m_strCli.c_str(),m_strDdi.c_str());
if(m_dbPackByMaster.size() == 0 ){
        setLastKnownError("No SQL Record Found");
        return retCode::BS_DB_NO_RECORD;
    }
    if(m_dbPackByMaster.size() < rowID && rowID < 0){
        setLastKnownError("Row ID out of Bound");
        return retCode::BS_ROW_OUTOFBOUND;
    }
  getData("m_custcode",rowID,m_dbPackByMaster,packByMaster.custcode);
  getData("m_batchcode",rowID,m_dbPackByMaster,packByMaster.batchcode);
  getData("m_serialcode",rowID,m_dbPackByMaster,packByMaster.serialcode);
  getData("package_id",rowID,m_dbPackByMaster,packByMaster.packageid);
  getData("card_id",rowID,m_dbPackByMaster,packByMaster.cardid);
  getData("uselimt_mode",rowID,m_dbPackByMaster,packByMaster.uselimitMode);
  getData("uselimt_delay",rowID,m_dbPackByMaster,packByMaster.uselimitDelay);
  getData("uselimt_amount",rowID,m_dbPackByMaster,packByMaster.uselimitamount);
  getData("uselimt_minutes",rowID,m_dbPackByMaster,packByMaster.uselimitminutes);
  getData("ispackcnxcharge",rowID,m_dbPackByMaster,packByMaster.ispackcnxcharge);
  getData("packfree_flag",rowID,m_dbPackByMaster,packByMaster.packfreeflag);
  //m_cdrUpdate->setFreeFlag(packByMaster.packfreeflag);
  getData("call_maxtalktime",rowID,m_dbPackByMaster,packByMaster.callMaxTalkTime);
  getData("call_maxtalktime_mode",rowID,m_dbPackByMaster,packByMaster.callMaxTalkTimeMode);
  getData("issubpackage",rowID,m_dbPackByMaster,packByMaster.isSubPackage);
  getData("promobal_ratio",rowID,m_dbPackByMaster,packByMaster.promobalRatio);
  getData("usagenotif_mode",rowID,m_dbPackByMaster,packByMaster.usagentnotifMode);
  getData("usagenotif_type",rowID,m_dbPackByMaster,packByMaster.usagentnotifType);
  getData("usagenotif_text",rowID,m_dbPackByMaster,packByMaster.usagentnotifText);
  getData("check_dest",rowID,m_dbPackByMaster,packByMaster.checkDest);
  getData("prefixdest",rowID,m_dbPackByMaster,packByMaster.prefixDest);
  getData("prefixcli_mode",rowID,m_dbPackByMaster,packByMaster.prefixCliMode);
  getData("check_cli",rowID,m_dbPackByMaster,packByMaster.checkCli);
  getData("usagenotif_threshold",rowID,m_dbPackByMaster,packByMaster.usagentnotifThreshold);
  getData("priceperunit",rowID,m_dbPackByMaster,packByMaster.pricePerUnit);
  getData("uselimit_call",rowID,m_dbPackByMaster,packByMaster.uselimitCall);
  getData("prefixdest_mode",rowID,m_dbPackByMaster,packByMaster.prefixDestMode);
  getData("balance",rowID,m_dbPackByMaster,packByMaster.balance);
  getData("breakage_charge_mode",rowID,m_dbPackByMaster,packByMaster.breakageChargeMode);
  getData("allow_roam",rowID,m_dbPackByMaster,packByMaster.allowRoam);

    return retCode::BS_SUCCESS;
}

retCode BSCharging::DB_esp_check_special_number(CA::DBInterface* dbInt,int cardid, std::string didnum, int nbtype) {
     logdebug(m_strCRF,"BSCharging::DB_esp_check_special_number() CLI %s DDI %s",m_strCli.c_str(),m_strDdi.c_str());
std::string spName = "esp_check_special_number";
std::string spValue  =  " '" + std::to_string(cardid) + "','" + didnum + "','" + std::to_string(nbtype) +"'" ;
CA::DBMAP dbcheck_special_number;
if(!dbInt){
        logerror(m_strCRF,"BSCharging::DB_esp_check_special_number() DB Connection failure");
         return retCode::BS_DB_CONNECTION_FAILED;
    }
    retCode ret = executeQuery(dbInt,spName,spValue,dbcheck_special_number);
    logdebug(m_strCRF,"BSCharging::%s() DataBase : %s,Query %s",__func__,getDataBaseName(dbInt->m_connectionString).c_str(),dbInt->getLastQuery().c_str());
    if(ret !=  retCode::BS_SUCCESS){
	logdebug(m_strCRF, "%sDB_esp_check_special_number DB:%s, Query:%s - failed %s", APP, m_strParamProfile.c_str(), dbInt->getLastQuery().c_str(), getLastKnownError().c_str());
        ClearDBData(dbcheck_special_number);
        return ret;
	}	
    if(dbcheck_special_number.size() == 0){
        setLastKnownError("No SQL Record Found");
        ClearDBData(dbcheck_special_number);
        return retCode::BS_DB_NO_RECORD;
    }
    ClearDBData(dbcheck_special_number);
    return ret;
}


retCode BSCharging::DB_MNP_get_info(CA::DBInterface* dbInt,std::string nb) {
    logdebug(m_strCRF,"BSCharging::DB_MNP_get_info() CLI %s DDI %s",m_strCli.c_str(),m_strDdi.c_str());
std::string spName = "MNP_GET_INFO";
std::string spValue  =  " '" + nb + "'" ;
CA::DBMAP dbMNP_info;
if(!dbInt){
        logerror(m_strCRF,"BSCharging::DB_MNP_get_info() DB Connection failure");
         return retCode::BS_DB_CONNECTION_FAILED;
    }
    retCode ret = executeQuery(dbInt,spName,spValue,dbMNP_info);
    logdebug(m_strCRF,"BSCharging::%s() DataBase : %s,Query %s",__func__,getDataBaseName(dbInt->m_connectionString).c_str(),dbInt->getLastQuery().c_str());
    if(ret !=  retCode::BS_SUCCESS){
	logdebug(m_strCRF, "%sDB_MNP_get_info DB:%s, Query:%s - failed %s", APP, m_strNpOprProfile.c_str(), dbInt->getLastQuery().c_str(), getLastKnownError().c_str());
        ClearDBData(dbMNP_info);
        return ret;
	}	
    if(dbMNP_info.size() == 0){
        setLastKnownError("No SQL Record Found");
        return retCode::BS_DB_NO_RECORD;
    }
    getData("mnp_number",0,dbMNP_info,m_mnpDet.mnpNum);
    getData("nrn_code",0,dbMNP_info,m_mnpDet.Nrn);
    ClearDBData(dbMNP_info);
    return ret;
}

retCode BSCharging::DB_esp_xlat_bybrand(CA::DBInterface* dbInt,std::string mnpNum) {
    logdebug(m_strCRF,"BSCharging::DB_esp_xlat_bybrand() CLI %s DDI %s",m_strCli.c_str(),m_strDdi.c_str());
std::string spName = "esp_xlat_bybrand";
std::string spValue  =  " '" + mnpNum + "'";
CA::DBMAP dbxlat_bybrand;
if(!dbInt){
        logerror(m_strCRF,"BSCharging::DB_esp_xlat_bybrand() DB Connection failure");
         return retCode::BS_DB_CONNECTION_FAILED;
    }
    retCode ret = executeQuery(dbInt,spName,spValue,dbxlat_bybrand);

    logdebug(m_strCRF,"BSCharging::%s() DataBase : %s,Query %s",__func__,getDataBaseName(dbInt->m_connectionString).c_str(),dbInt->getLastQuery().c_str());
    if(ret !=  retCode::BS_SUCCESS){
	logdebug(m_strCRF, "%sDB_esp_xlat_bybrand DB:%s, Query:%s - failed %s", APP, m_strAccountProfile.c_str(), dbInt->getLastQuery().c_str(), getLastKnownError().c_str());
        ClearDBData(dbxlat_bybrand);
        return ret;
	}	
    if(dbxlat_bybrand.size() == 0){
        setLastKnownError("No SQL Record Found");
        return retCode::BS_DB_NO_RECORD;
    }
    getData("prefix_new",0,dbxlat_bybrand,m_byBrandxlat.prefix_new);
	getData("prefix_old",0,dbxlat_bybrand,m_byBrandxlat.prefix_old);
    ClearDBData(dbxlat_bybrand);
    return ret;
}

retCode BSCharging::DB_es3_get_prefix(CA::DBInterface* dbInt,std::string sitecode, std::string destnum, int prefix_set) {
    logdebug(m_strCRF,"BSCharging::DB_es3_get_prefix() CLI %s DDI %s",m_strCli.c_str(),m_strDdi.c_str());
std::string spName = "es3_get_prefix";
std::string spValue  =  " '" + sitecode +"','" + destnum + "'," + std::to_string(prefix_set);
CA::DBMAP dbprefix;
if(!dbInt){
        logerror(m_strCRF,"BSCharging::DB_es3_get_prefix() DB Connection failure");
         return retCode::BS_DB_CONNECTION_FAILED;
    }
    retCode ret = executeQuery(dbInt,spName,spValue,dbprefix);

    logdebug(m_strCRF,"BSCharging::%s() DataBase : %s,Query %s",__func__,getDataBaseName(dbInt->m_connectionString).c_str(),dbInt->getLastQuery().c_str());
    if(ret !=  retCode::BS_SUCCESS){
  	logdebug(m_strCRF, "%sDB_es3_get_prefix DB:%s, Query:%s - failed %s", APP, m_strParamProfile.c_str(), dbInt->getLastQuery().c_str(), getLastKnownError().c_str());
        ClearDBData(dbprefix);
        return ret;
	}	
    if(dbprefix.size() == 0){
        setLastKnownError("No SQL Record Found");
        return retCode::BS_DB_NO_RECORD;
    }
    getData("destcode",0,dbprefix,m_mnpDet.destcode);
    ClearDBData(dbprefix);
    return ret;
}

retCode BSCharging::DB_esp_get_tariffcode(CA::DBInterface* dbInt,std::string destnum) {
    logdebug(m_strCRF,"BSCharging::DB_esp_get_tariffcode() CLI %s DDI %s",m_strCli.c_str(),m_strDdi.c_str());
std::string spName = "esp_get_tariffcode";
std::string spValue  =  " '" + destnum + "'";
CA::DBMAP dbtariffcode;
if(!dbInt){
        logerror(m_strCRF,"BSCharging::DB_esp_get_tariffcode() DB Connection failure");
         return retCode::BS_DB_CONNECTION_FAILED;
    }
    retCode ret = executeQuery(dbInt,spName,spValue,dbtariffcode);
    logdebug(m_strCRF,"BSCharging::%s() DataBase : %s,Query %s",__func__,getDataBaseName(dbInt->m_connectionString).c_str(),dbInt->getLastQuery().c_str());
    if(ret !=  retCode::BS_SUCCESS){		
	logdebug(m_strCRF, "%sDB_esp_get_tariffcode DB:%s, Query:%s - failed %s", APP, m_strNpOprProfile.c_str(), dbInt->getLastQuery().c_str(), getLastKnownError().c_str());
        ClearDBData(dbtariffcode);
        return ret;
	}	
    if(dbtariffcode.size() == 0){
        setLastKnownError("No SQL Record Found");
        return retCode::BS_DB_NO_RECORD;
    }
    getData("tariff_code",0,dbtariffcode,m_tariffCode.tariff_code);
    ClearDBData(dbtariffcode);
    return ret;
}

retCode BSCharging::DB_esp_checkroaming_dest(CA::DBInterface* dbInt, std::string sitecode, std::string dest_code, std::string xlatednum) {
    logdebug(m_strCRF,"BSCharging::DB_esp_checkroaming_dest() CLI %s DDI %s",m_strCli.c_str(),m_strDdi.c_str());
std::string spName = "esp_checkroaming_dest";
std::string spValue  =  " '" + sitecode +"','" + dest_code + "','" + xlatednum + "'";
CA::DBMAP dbcheckroaming_dest;
if(!dbInt){
        logerror(m_strCRF,"BSCharging::DB_esp_checkroaming_dest() DB Connection failure");
         return retCode::BS_DB_CONNECTION_FAILED;
    }
    retCode ret = executeQuery(dbInt,spName,spValue,dbcheckroaming_dest);

    logdebug(m_strCRF,"BSCharging::%s() DataBase : %s,Query %s",__func__,getDataBaseName(dbInt->m_connectionString).c_str(),dbInt->getLastQuery().c_str());
    if(ret !=  retCode::BS_SUCCESS){
	  	logdebug(m_strCRF, "%sDB_esp_checkroaming_dest DB:%s, Query:%s - failed %s", APP, m_strAccountProfile.c_str(), dbInt->getLastQuery().c_str(), getLastKnownError().c_str());
        ClearDBData(dbcheckroaming_dest);
        return ret;
	}	
    if(dbcheckroaming_dest.size() == 0){
        setLastKnownError("No SQL Record Found");
        return retCode::BS_DB_NO_RECORD;
    }
    ClearDBData(dbcheckroaming_dest);
    return ret;
}


retCode BSCharging::DB_esp_is_prefixgroup_blocked_bycli(CA::DBInterface* dbInt,std::string a_cli, std::string xlatednum) {
    logdebug(m_strCRF,"BSCharging::DB_esp_is_prefixgroup_blocked_bycli() CLI %s DDI %s",m_strCli.c_str(),m_strDdi.c_str());
std::string spName = "esp_is_prefixgroup_blocked_bycli";
std::string spValue  = " '" + a_cli + "','" + xlatednum + "'";
CA::DBMAP dbprefixgroup_blocked_bycli;
if(!dbInt){
        logerror(m_strCRF,"BSCharging::DB_esp_is_prefixgroup_blocked_bycli() DB Connection failure");
         return retCode::BS_DB_CONNECTION_FAILED;
    }
    retCode ret = executeQuery(dbInt,spName,spValue,dbprefixgroup_blocked_bycli);
    logdebug(m_strCRF,"BSCharging::%s() DataBase : %s,Query %s",__func__,getDataBaseName(dbInt->m_connectionString).c_str(),dbInt->getLastQuery().c_str());
    if(ret !=  retCode::BS_SUCCESS){
    logdebug(m_strCRF, "%sDB_esp_is_prefixgroup_blocked_bycli DB:%s, Query:%s - failed %s", APP, m_strAccountProfile.c_str(), dbInt->getLastQuery().c_str(), getLastKnownError().c_str());
        ClearDBData(dbprefixgroup_blocked_bycli);
        return ret;
	}		
    if(dbprefixgroup_blocked_bycli.size() == 0){
        setLastKnownError("No SQL Record Found");
        return retCode::BS_DB_NO_RECORD;
    }
    ClearDBData(dbprefixgroup_blocked_bycli);
    return ret;
}


retCode BSCharging::DB_es_isprefixblocked(CA::DBInterface* dbInt,std::string xlatednum) {

    logdebug(m_strCRF,"BSCharging::DB_es_isprefixblocked() CLI %s DDI %s",m_strCli.c_str(),m_strDdi.c_str());
std::string spName = "es_isprefixblocked";
std::string spValue  = " '" +  xlatednum + "'";
CA::DBMAP dbprefixblocked;
if(!dbInt){
        logerror(m_strCRF,"BSCharging::DB_es_isprefixblocked() DB Connection failure");
         return retCode::BS_DB_CONNECTION_FAILED;
    }
    retCode ret = executeQuery(dbInt,spName,spValue,dbprefixblocked);

    logdebug(m_strCRF,"BSCharging::%s() DataBase : %s,Query %s",__func__,getDataBaseName(dbInt->m_connectionString).c_str(),dbInt->getLastQuery().c_str());
    if(ret !=  retCode::BS_SUCCESS){
	logdebug(m_strCRF, "%sDB_es_isprefixblocked DB:%s, Query:%s - failed %s", APP, m_strParamProfile.c_str(), dbInt->getLastQuery().c_str(), getLastKnownError().c_str());
        ClearDBData(dbprefixblocked);
        return ret;
	}	
    if(dbprefixblocked.size() == 0){
        setLastKnownError("No SQL Record Found");
        ClearDBData(dbprefixblocked);
        return retCode::BS_DB_NO_RECORD;
    }
    	getData("prefixcode",0,dbprefixblocked,m_mnpDet.isPrefixCodeBlocked);
		getData("destcode",0,dbprefixblocked,m_mnpDet.blockedprefixdestcode);
    ClearDBData(dbprefixblocked);
    return ret;
}
retCode BSCharging::DB_es1_get_premiumnb(CA::DBInterface* dbInt,std::string nb) {
    logdebug(m_strCRF,"BSCharging::DB_es1_get_premiumnb() CLI %s DDI %s",m_strCli.c_str(),m_strDdi.c_str());
std::string spName = "es1_get_premiumnb";
std::string spValue  = " '" + nb + "'";
CA::DBMAP dbpremiumnb;
if(!dbInt){
        logerror(m_strCRF,"BSCharging::DB_es1_get_premiumnb() DB Connection failure");
         return retCode::BS_DB_CONNECTION_FAILED;
    }
    retCode ret = executeQuery(dbInt,spName,spValue,dbpremiumnb);

    logdebug(m_strCRF,"BSCharging::%s() DataBase : %s,Query %s",__func__,getDataBaseName(dbInt->m_connectionString).c_str(),dbInt->getLastQuery().c_str());
    if(ret !=  retCode::BS_SUCCESS){
	logdebug(m_strCRF, "%sDB_es1_get_premiumnb DB:%s, Query:%s - failed %s", APP, m_strParamProfile.c_str(), dbInt->getLastQuery().c_str(), getLastKnownError().c_str());
        ClearDBData(dbpremiumnb);
        return ret;
	}	
    if(dbpremiumnb.size() == 0){
        setLastKnownError("No SQL Record Found");
        return retCode::BS_DB_NO_RECORD;
    }
    ClearDBData(dbpremiumnb);
    return ret;
}


retCode BSCharging::DB_es2_gettimeclass(CA::DBInterface* dbInt,std::string sitecode, std::string tariff_class) {
    logdebug(m_strCRF,"BSCharging::DB_es2_gettimeclass() CLI %s DDI %s",m_strCli.c_str(),m_strDdi.c_str());
std::string spName = "es2_gettimeclass";
std::string time = GetTime();
std::string spValue  = " '" + sitecode + "','" + time + "',1,'" + tariff_class + "'";
CA::DBMAP dbtimeclass;
if(!dbInt){
        logerror(m_strCRF,"BSCharging::DB_es2_gettimeclass() DB Connection failure");
         return retCode::BS_DB_CONNECTION_FAILED;
    }
    retCode ret = executeQuery(dbInt,spName,spValue,dbtimeclass);
    logdebug(m_strCRF,"BSCharging::%s() DataBase : %s,Query %s",__func__,getDataBaseName(dbInt->m_connectionString).c_str(),dbInt->getLastQuery().c_str());
    if(ret !=  retCode::BS_SUCCESS){	
	logdebug(m_strCRF, "%sDB_es2_gettimeclass DB:%s, Query:%s - failed %s", APP, m_strParamProfile.c_str(), dbInt->getLastQuery().c_str(), getLastKnownError().c_str());
        ClearDBData(dbtimeclass);
        return ret;
	}	
    if(dbtimeclass.size() == 0){
        setLastKnownError("No SQL Record Found");
        return retCode::BS_DB_NO_RECORD;
    }
    getData("timeclass",0,dbtimeclass,m_timeClass.time_prd);
    ClearDBData(dbtimeclass);
    return ret;
}


retCode BSCharging::DB_es2_get_tariff(CA::DBInterface* dbparamInt,std::string telco_code, std::string sitecode, 
		std::string dest_code, std::string tariff_class, int access_charge, int time_prd, int routeclass,GetTariff &getTariff) {
            logdebug(m_strCRF,"BSCharging::DB_es2_get_tariff() CLI %s DDI %s",m_strCli.c_str(),m_strDdi.c_str());
std::string spName = "es2_get_tariff";
std::string spValue  = " '" + telco_code + "','" + sitecode + "','" + dest_code + "','" + tariff_class + "'," + std::to_string(access_charge) +"," + std::to_string(time_prd) + ",0"; // defafult param 1 added for mysql support
CA::DBMAP dbtariff;
getTariff.no_package = 0;
if(!dbparamInt){
        logerror(m_strCRF,"BSCharging::DB_es2_get_tariff() DB Connection failure");
	getTariff.no_package = 1;
         return retCode::BS_DB_CONNECTION_FAILED;
    }
    retCode ret = executeQuery(dbparamInt,spName,spValue,dbtariff);

    logdebug(m_strCRF,"BSCharging::%s() DataBase : %s,Query %s",__func__,getDataBaseName(dbparamInt->m_connectionString).c_str(),dbparamInt->getLastQuery().c_str());
    if(ret !=  retCode::BS_SUCCESS){
	logdebug(m_strCRF, "%sDB_es2_get_tariff DB:%s, Query:%s - failed %s", APP, m_strParamProfile.c_str(), dbparamInt->getLastQuery().c_str(), getLastKnownError().c_str());
	getTariff.no_package = 1;
        ClearDBData(dbtariff);
        return ret;
	}	
    if(dbtariff.size() == 0){
        setLastKnownError("No SQL Record Found");
	getTariff.no_package = 1;
        return retCode::BS_DB_NO_RECORD;
    }
    getData("cnxunit",0,dbtariff,getTariff.cnxunit);
    getData("premiumdest",0,dbtariff,getTariff.preDest);
    getData("sampdelay",0,dbtariff,getTariff.sampledelay);
    getData("sampunit",0,dbtariff,getTariff.sampleunit);
    getData("cnxdelay",0,dbtariff,getTariff.cnxdelay);
    std::cout<<"***cnxunit-("<<getTariff.cnxunit<<"), premiumdest-("<<getTariff.preDest<<"), sampdelay-("<<getTariff.sampledelay<<"), sampunit-("<<getTariff.sampleunit<<"), cnxdelay-("<<getTariff.cnxdelay<<")***"<<std::endl;
    ClearDBData(dbtariff);
    return ret;
}
retCode BSCharging::DB_es1_get_progsampl(CA::DBInterface* dbInt,std::string telco_code, std::string site_code, std::string mast_tariff_class,std::string dest_code, int charge, int period, int talktime){
    logdebug(m_strCRF,"BSCharging::DB_es1_get_progsampl() CLI %s DDI %s",m_strCli.c_str(),m_strDdi.c_str());
std::string spName = "es1_get_progsampl";
std::string spValue  = "'"+telco_code + "','" + site_code + "','"+mast_tariff_class + "','"+dest_code +"',"+ std::to_string(charge)+ ","+ std::to_string(period)+","+std::to_string(talktime);
CA::DBMAP dbprogsampl;
if(!dbInt){
        logerror(m_strCRF,"BSCharging::DB_es1_get_progsampl() DB Connection failure");
         return retCode::BS_DB_CONNECTION_FAILED;
    }
    retCode ret = executeQuery(dbInt,spName,spValue,dbprogsampl);

    logdebug(m_strCRF,"BSCharging::%s() DataBase : %s,Query %s",__func__,getDataBaseName(dbInt->m_connectionString).c_str(),dbInt->getLastQuery().c_str());
    if(ret !=  retCode::BS_SUCCESS){
	 logdebug(m_strCRF, "%sDB_es1_get_progsampl DB:%s, Query:%s - failed %s", APP, m_strParamProfile.c_str(), dbInt->getLastQuery().c_str(), getLastKnownError().c_str());
        ClearDBData(dbprogsampl);
        return ret;
	}	
    if(dbprogsampl.size() == 0){
        setLastKnownError("No SQL Record Found");
        return retCode::BS_DB_NO_RECORD;
    }
    ClearDBData(dbprogsampl);
    return ret;
}


retCode BSCharging::DB_esp_get_package_exception(CA::DBInterface* dbInt,std::string sitecode, int mvno_brandid, 
	std::string tariff_class, int package_id, std::string dest_code, std::string xlated_num) {
        logdebug(m_strCRF,"BSCharging::DB_esp_get_package_exception() CLI %s DDI %s",m_strCli.c_str(),m_strDdi.c_str());
std::string spName = "esp_get_package_exception";
std::string spValue  = " '" + sitecode + "'," + std::to_string(mvno_brandid) + ",'" + tariff_class + "'," + std::to_string(package_id) + ",'" + dest_code + "','" + xlated_num + "'";
CA::DBMAP dbpackage_exception;
if(!dbInt){
        logerror(m_strCRF,"BSCharging::DB_esp_get_package_exception() DB Connection failure");
         return retCode::BS_DB_CONNECTION_FAILED;
    }
    retCode ret = executeQuery(dbInt,spName,spValue,dbpackage_exception);

    logdebug(m_strCRF,"BSCharging::%s() DataBase : %s,Query %s",__func__,getDataBaseName(dbInt->m_connectionString).c_str(),dbInt->getLastQuery().c_str());
    if(ret !=  retCode::BS_SUCCESS){
	 logdebug(m_strCRF, "%sDB_esp_get_package_exception DB:%s, Query:%s - failed %s", APP, m_strAccountProfile.c_str(), dbInt->getLastQuery().c_str(), getLastKnownError().c_str());
        ClearDBData(dbpackage_exception);
        return ret;
	}	
    if(dbpackage_exception.size() == 0){
        setLastKnownError("No SQL Record Found");
        return retCode::BS_DB_NO_RECORD;
    }
    ClearDBData(dbpackage_exception);
    return ret;
}


retCode BSCharging::DB_esp_package_authorize_dest_insertlog(CA::DBInterface* dbInt,std::string telco_code, std::string cust_code,
	int batch_code, int serial_code, std::string bun_custcode, int bun_batchcode, int bun_serialcode,
	int package_id, std::string destMSISDN) {
        logdebug(m_strCRF,"BSCharging::DB_esp_package_authorize_dest_insertlog() CLI %s DDI %s",m_strCli.c_str(),m_strDdi.c_str());
std::string spName = "esp_package_authorize_dest_insertlog";
std::string time = GetTime();
std::string spValue  =  " '"+ telco_code + "','" + cust_code + "'," + std::to_string(batch_code) + "," + std::to_string(serial_code) + ",'" + bun_custcode + "'," 
+ std::to_string(bun_batchcode) +"," + std::to_string(bun_serialcode) + "," + std::to_string(package_id) + ",'" + destMSISDN + "','" + time + "','ESP'";
CA::DBMAP dbpackage_authorize_dest_insertlog;
if(!dbInt){
        logerror(m_strCRF,"BSCharging::DB_esp_package_authorize_dest_insertlog() DB Connection failure");
         return retCode::BS_DB_CONNECTION_FAILED;
    }
    retCode ret = executeQuery(dbInt,spName,spValue,dbpackage_authorize_dest_insertlog);

    logdebug(m_strCRF,"BSCharging::%s() DataBase : %s,Query %s",__func__,getDataBaseName(dbInt->m_connectionString).c_str(),dbInt->getLastQuery().c_str());
    if(ret !=  retCode::BS_SUCCESS){
	  logdebug(m_strCRF, "%sDB_esp_package_authorize_dest_insertlog DB:%s, Query:%s - failed %s", APP, m_strAccountProfile.c_str(), dbInt->getLastQuery().c_str(), getLastKnownError().c_str());
         ClearDBData(dbpackage_authorize_dest_insertlog);
        return ret;
	}	
    if(dbpackage_authorize_dest_insertlog.size() == 0){
        setLastKnownError("No SQL Record Found");
        return retCode::BS_DB_NO_RECORD;
    }
    ClearDBData(dbpackage_authorize_dest_insertlog);
    return ret;
}

retCode BSCharging::DB_es5_update_ccaas_balance_infront(CA::DBInterface* dbInt, BSParam* bsp){
    logdebug(m_strCRF,"BSCharging::%s() CLI %s DDI %s",__func__,m_strCli.c_str(),m_strDdi.c_str());
    std::string spName   = "ccaas_update_callcost_session_history_details";
    std::string uid      =  bsp->getUid();
    double callCost      = bsp->getUnitsCharged();
    std::string spValue  = " '" + uid + "'," + std::to_string(callCost) ;
    CA::DBMAP dbupdate_ccaas_balance_infront;
    if(!dbInt){
       logerror(m_strCRF,"BSCharging::%s() DB Connection failure",__func__);
       return retCode::BS_DB_CONNECTION_FAILED;
      }
      retCode ret = executeQuery(dbInt,spName,spValue,dbupdate_ccaas_balance_infront);
      logdebug(m_strCRF,"BSCharging::%s() DataBase : %s,Query %s",__func__,getDataBaseName(dbInt->m_connectionString).c_str(),dbInt->getLastQuery().c_str());
      if(ret !=  retCode::BS_SUCCESS){
         logerror(m_strCRF, "BSCharging::%s() DB:%s, Query:%s - failed %s",__func__, m_strAccountProfile.c_str(), dbInt->getLastQuery().c_str(), getLastKnownError().c_str());
         ClearDBData(dbupdate_ccaas_balance_infront);
         return ret;
        }
    ClearDBData(dbupdate_ccaas_balance_infront);
    return ret;
}

retCode BSCharging::DB_es5_update_balance_infront(CA::DBInterface* dbInt,std::string telco_code, std::string cust_code,
	int batch_code, int serial_code, double v_allocbal, double account_cnxcount, int nof_charge, 
	std::string switchcode, std::string task_id) {
     logdebug(m_strCRF,"BSCharging::DB_es5_update_balance_infront() CLI %s DDI %s",m_strCli.c_str(),m_strDdi.c_str());
std::string spName = "es5_update_balance_infront";
std::string time  =  GetTime();
std::string spValue  = " '" + telco_code + "','" + cust_code + "'," + std::to_string(batch_code) + "," + std::to_string(serial_code) + "," + std::to_string(v_allocbal) + 
"," + std::to_string(0) + "," + std::to_string(0) + "," + std::to_string(0) + ",'" + time + "'," +std::to_string(1) + ",'" + m_strSiteCode + "','" + switchcode + "','" + 
task_id + "'";
CA::DBMAP dbupdate_balance_infrontl;
if(!dbInt){
        logerror(m_strCRF,"BSCharging::DB_es5_update_balance_infront DB Connection failure");
         return retCode::BS_DB_CONNECTION_FAILED;
    }
    retCode ret = executeQuery(dbInt,spName,spValue,dbupdate_balance_infrontl);

    logdebug(m_strCRF,"BSCharging::%s() DataBase : %s,Query %s",__func__,getDataBaseName(dbInt->m_connectionString).c_str(),dbInt->getLastQuery().c_str());
    if(ret !=  retCode::BS_SUCCESS){
	logdebug(m_strCRF, "%sDB_es5_update_balance_infront DB:%s, Query:%s - failed %s", APP, m_strAccountProfile.c_str(), dbInt->getLastQuery().c_str(), getLastKnownError().c_str());
        ClearDBData(dbupdate_balance_infrontl);
        return ret;
	}	
    if(dbupdate_balance_infrontl.size() == 0){
        setLastKnownError("No SQL Record Found");
        return retCode::BS_DB_NO_RECORD;
    }
    getData("balance",0,dbupdate_balance_infrontl,m_updateBalInFront.balance);
    getData("balanceused",0,dbupdate_balance_infrontl,m_updateBalInFront.balanceused);
    getData("cnxcount",0,dbupdate_balance_infrontl,m_updateBalInFront.cnxcount);
    getData("totalcons",0,dbupdate_balance_infrontl,m_updateBalInFront.totalcons);
    std::cout<<"***balance-("<<m_updateBalInFront.balance<<"), balanceused-("<<m_updateBalInFront.balanceused<<"), totalcons-("<<m_updateBalInFront.totalcons<<")***"<<std::endl;
    ClearDBData(dbupdate_balance_infrontl);
    return ret;
}

retCode BSCharging::DB_esp_update_packagelimit_v2(CA::DBInterface* dbInt,std::string cust_code,
	int batch_code, int serial_code,int limit_mode,int limit_delay,double v_allocabal,int v_min){
        logdebug(m_strCRF,"BSCharging::DB_esp_update_packagelimit_v2() CLI %s DDI %s",m_strCli.c_str(),m_strDdi.c_str());
        std::string time = GetTime();
        if(!dbInt){
        logerror(m_strCRF,"BSCharging::DB_esp_update_packagelimit_v2() DB Connection failure");
         return retCode::BS_DB_CONNECTION_FAILED;
    }
    if(limit_mode == 1){
        std::string spName = "exec esp_update_packagelimit";
        std::string spValue  = " '" + cust_code + "'," + std::to_string(batch_code) + "," + std::to_string(serial_code) + ",'" + time + "'," +
                                    std::to_string(limit_mode) + "," + std::to_string(limit_delay) + "," + std::to_string(v_allocabal) + ","+
                                    std::to_string(0);
        CA::DBMAP dbupdate_packagelimit;
        retCode ret = executeQuery(dbInt,spName,spValue,dbupdate_packagelimit);
        logdebug(m_strCRF,"BSCharging::%s() DataBase : %s,Query %s",__func__,getDataBaseName(dbInt->m_connectionString).c_str(),dbInt->getLastQuery().c_str());

        if(ret !=  retCode::BS_SUCCESS){
		logdebug(m_strCRF, "%sDB_esp_update_packagelimit_v2 DB:%s, Query:%s - failed %s", APP, m_strAccountProfile.c_str(), dbInt->getLastQuery().c_str(), getLastKnownError().c_str());
            ClearDBData(dbupdate_packagelimit);
            return ret;
		}	
        if(dbupdate_packagelimit.size() == 0){
            setLastKnownError("No SQL Record Found");
            return retCode::BS_DB_NO_RECORD;
        }
        ClearDBData(dbupdate_packagelimit);
        return ret;
    }else if(limit_mode == 2){
        std::string spName = "exec esp_update_packagelimit";
        std::string spValue  =  " '" + cust_code + "'," +
                                    std::to_string(batch_code) + "," + std::to_string(serial_code) + ",'" + time + "'," +
                                    std::to_string(limit_mode) + "," + std::to_string(limit_delay) + "," + std::to_string(0) + ","+
                                    std::to_string((v_min*60)/60);
        CA::DBMAP dbupdate_packagelimit;
        retCode ret = executeQuery(dbInt,spName,spValue,dbupdate_packagelimit);
        logdebug(m_strCRF,"BSCharging::%s() DataBase : %s,Query %s",__func__,getDataBaseName(dbInt->m_connectionString).c_str(),dbInt->getLastQuery().c_str());
        if(ret !=  retCode::BS_SUCCESS){
		 logdebug(m_strCRF, "%sDB_esp_update_packagelimit_v2 DB:%s, Query:%s - failed %s", APP, m_strParamProfile.c_str(), dbInt->getLastQuery().c_str(), getLastKnownError().c_str());
            ClearDBData(dbupdate_packagelimit);
            return ret;
		}	
        if(dbupdate_packagelimit.size() == 0){
            setLastKnownError("No SQL Record Found");
            return retCode::BS_DB_NO_RECORD;
        }
        ClearDBData(dbupdate_packagelimit);
        return ret;
    }
    return retCode::BS_ERROR_UNKOWN;
}

retCode BSCharging::DB_esp_get_package_prefixdest(CA::DBInterface* dbInt,std::string telco_code, 
	std::string cust_code, int batch_code, int serial_code, std::string a_cli, 
	std::string &inxlatednum) {
  logdebug(m_strCRF,"BSCharging::DB_esp_get_package_prefixdest() CLI %s DDI %s",m_strCli.c_str(),m_strDdi.c_str());
std::string spName = "esp_get_package_prefixdest";
std::string time = GetTime();
std::string spValue  =  " '" +
	                	telco_code + "','" + cust_code + "'," + std::to_string(batch_code) + "," + 
		                std::to_string(serial_code) + ",0,'" + time +
		                "','" + a_cli + "','" + inxlatednum + "'";
CA::DBMAP dbpackage_prefixdest;
if(!dbInt){
        logerror(m_strCRF,"BSCharging::DB_esp_get_package_prefixdest() DB Connection failure");
         return retCode::BS_DB_CONNECTION_FAILED;
    }
    retCode ret = executeQuery(dbInt,spName,spValue,dbpackage_prefixdest);

    logdebug(m_strCRF,"BSCharging::%s() DataBase : %s,Query %s",__func__,getDataBaseName(dbInt->m_connectionString).c_str(),dbInt->getLastQuery().c_str());
    if(ret !=  retCode::BS_SUCCESS){
	logdebug(m_strCRF, "%sDB_esp_get_package_prefixdest DB:%s, Query:%s - failed %s", APP, m_strAccountProfile.c_str(), dbInt->getLastQuery().c_str(), getLastKnownError().c_str());
       ClearDBData(dbpackage_prefixdest);
        return ret;
	}	
    if(dbpackage_prefixdest.size() == 0){
        setLastKnownError(" esp_get_package_prefixdest - No SQL Record Found");
        return retCode::BS_DB_NO_RECORD;
    }
    std::string prexlat;
    getData("prefixdest",0,dbpackage_prefixdest,prexlat);
    ClearDBData(dbpackage_prefixdest);
    return ret;
}


retCode BSCharging::DB_es1_max_sampunit(CA::DBInterface* dbInt,std::string tele_code, std::string sitecode, std::string tariffclass,std::string dest_code , int access_charge, int time_prd) {
    logdebug(m_strCRF,"BSCharging::DB_es1_max_sampunit() CLI %s DDI %s",m_strCli.c_str(),m_strDdi.c_str());
std::string spName = "es1_max_sampunit";
std::string spValue  =  " '" + tele_code +  "','" + sitecode + "','" + tariffclass + "','" + dest_code + "'," + 
		               std::to_string(access_charge) + "," + std::to_string(time_prd);
CA::DBMAP dbmax_sampunit;
if(!dbInt){
        logerror(m_strCRF,"BSCharging::DB_es1_max_sampunit() DB Connection failure");
         return retCode::BS_DB_CONNECTION_FAILED;
    }
    retCode ret = executeQuery(dbInt,spName,spValue,dbmax_sampunit);

    logdebug(m_strCRF,"BSCharging::%s() DataBase : %s,Query %s",__func__,getDataBaseName(dbInt->m_connectionString).c_str(),dbInt->getLastQuery().c_str());
    if(ret !=  retCode::BS_SUCCESS){
	logdebug(m_strCRF, "%sDB_es1_max_sampunit DB:%s, Query:%s - failed %s", APP, m_strAccountProfile.c_str(), dbInt->getLastQuery().c_str(), getLastKnownError().c_str());
        ClearDBData(dbmax_sampunit);
        return ret;
	}	
    if(dbmax_sampunit.size() == 0){
        setLastKnownError("No SQL Record Found");
        return retCode::BS_DB_NO_RECORD;
    }
    ClearDBData(dbmax_sampunit);
    return ret;
}


retCode BSCharging::DB_esp_get_balance_infront(CA::DBInterface* dbInt,std::string sitecode,std::string switchcode, int task_id, std::string telco_code, std::string cust_code, int batch_code, int serial_code,int conf_talkcharge){
    logdebug(m_strCRF,"BSCharging::DB_esp_get_balance_infront() CLI %s DDI %s",m_strCli.c_str(),m_strDdi.c_str());
std::string spName = "esp_get_balance_infront";
std::string spValue  =   " '" + 
		               sitecode + "','" + switchcode + "'," + std::to_string(task_id) + ",'" + telco_code + 
		               "','" + cust_code + "'," + std::to_string(batch_code) + "," + std::to_string(serial_code) +
		               ",1,0.0" + std::to_string(conf_talkcharge);
CA::DBMAP dbbalance_infront;
if(!dbInt){
        logerror(m_strCRF,"BSCharging::DB_esp_get_balance_infront() DB Connection failure");
         return retCode::BS_DB_CONNECTION_FAILED;
    }
    retCode ret = executeQuery(dbInt,spName,spValue,dbbalance_infront);
    logdebug(m_strCRF,"BSCharging::%s() DataBase : %s,Query %s",__func__,getDataBaseName(dbInt->m_connectionString).c_str(),dbInt->getLastQuery().c_str());
    if(ret !=  retCode::BS_SUCCESS){
	logdebug(m_strCRF, "%sDB_esp_get_balance_infront DB:%s, Query:%s - failed %s", APP,m_strAccountProfile.c_str(), m_strParamProfile, dbInt->getLastQuery().c_str(), getLastKnownError().c_str());
        ClearDBData(dbbalance_infront);
        return ret;
	}	
    if(dbbalance_infront.size() == 0){
        setLastKnownError("No SQL Record Found");
        return retCode::BS_DB_NO_RECORD;
    }
    ClearDBData(dbbalance_infront);
    return ret;
}


retCode BSCharging::DB_esp_package_authorize_dest(CA::DBInterface* dbInt,std::string telco_code, std::string cust_code,
	int batch_code, int serial_code, std::string bun_custcode, int bun_batchcode, int bun_serialcode,
	int package_id, std::string destMSISDN) {
        logdebug(m_strCRF,"BSCharging::DB_esp_package_authorize_dest() CLI %s DDI %s",m_strCli.c_str(),m_strDdi.c_str());
std::string time = GetTime();
	
    std::string spName =  "esp_package_authorize_dest";
    std::string spValue = "'" +  telco_code + "','" + cust_code + "'," + std::to_string(batch_code) + "," + std::to_string(serial_code) + ",'" + 
		                bun_custcode + "'," + std::to_string(bun_batchcode) + "," + std::to_string(bun_serialcode) + "," + 
		                std::to_string(package_id) + ",'" + destMSISDN + "','" + time + "','ESP'";
    
    
	CA::DBMAP dbespPackageAuthDest;
    if(!dbInt){
        logerror(m_strCRF,"BSCharging::DB_esp_package_authorize_dest() DB Connection failure");
         return retCode::BS_DB_CONNECTION_FAILED;
    }
    retCode ret =  executeQuery(dbInt,spName,spValue,dbespPackageAuthDest);
    logdebug(m_strCRF,"BSCharging::%s() DataBase : %s,Query %s",__func__,getDataBaseName(dbInt->m_connectionString).c_str(),dbInt->getLastQuery().c_str());
    if(ret != retCode::BS_SUCCESS){
		   	logdebug(m_strCRF, "%sDB_esp_package_authorize_dest DB:%s, Query:%s - failed %s", APP, m_strAccountProfile.c_str(), dbInt->getLastQuery().c_str(), getLastKnownError().c_str());
        ClearDBData(dbespPackageAuthDest);
        return ret;
    }
    getData("action",0,dbespPackageAuthDest,m_authDest.action);
    getData("prompt_file",0,dbespPackageAuthDest,m_authDest.prompt_file);
    getData("errmessage",0,dbespPackageAuthDest,m_authDest.errmsg);
    ClearDBData(dbespPackageAuthDest);
    return retCode::BS_SUCCESS ;
}



retCode BSCharging::DB_esp_package_bundle_changedate(CA::DBInterface* dbInt,std::string sitecode,std::string switchcode, int task_id, std::string telco_code, std::string cust_code, int batch_code, int serial_code,int resmon_packid,int usertype,std::string xlatednum ) {
    logdebug(m_strCRF,"BSCharging::DB_esp_package_bundle_changedate() CLI %s DDI %s",m_strCli.c_str(),m_strDdi.c_str());
std::string spName = "esp_package_bundle_changedate";
std::string time = GetTime();
std::string spValue  =   " '" +
		               sitecode + "','" + telco_code + "','" + cust_code + "'," + std::to_string(batch_code) + 
		               "," + std::to_string(serial_code) + "," + std::to_string(resmon_packid) + "," + 
		               std::to_string(usertype) + ",'" + xlatednum + "','" + time + "','ESP'";
CA::DBMAP dbpackage_bundle_changedate;
if(!dbInt){
        logerror(m_strCRF,"BSCharging::DB_esp_package_bundle_changedate() DB Connection failure");
         return retCode::BS_DB_CONNECTION_FAILED;
    }
    retCode ret = executeQuery(dbInt,spName,spValue,dbpackage_bundle_changedate);
    logdebug(m_strCRF,"BSCharging::%s() DataBase : %s,Query %s",__func__,getDataBaseName(dbInt->m_connectionString).c_str(),dbInt->getLastQuery().c_str());
    if(ret !=  retCode::BS_SUCCESS){
	logdebug(m_strCRF, "%sDB_esp_package_bundle_changedate DB:%s, Query:%s - failed %s", APP,m_strAccountProfile.c_str(), dbInt->getLastQuery().c_str(), getLastKnownError().c_str());
        ClearDBData(dbpackage_bundle_changedate);
        return ret;
	}	
    if(dbpackage_bundle_changedate.size() == 0){
        setLastKnownError("No SQL Record Found");
        ClearDBData(dbpackage_bundle_changedate);
        return retCode::BS_DB_NO_RECORD;
    }
    ClearDBData(dbpackage_bundle_changedate);
    return ret;
}


retCode BSCharging::DB_esp_get_sms_campaign(CA::DBInterface* dbInt,std::string sitecode,std::string mvno_phone,std::string xlated_num) {
    logdebug(m_strCRF,"BSCharging::DB_esp_get_sms_campaign() CLI %s DDI %s",m_strCli.c_str(),m_strDdi.c_str());
std::string spName = "esp_get_sms_campaign";
std::string time = GetTime();
std::string spValue  = " '" + sitecode +
		               "','" + mvno_phone + "','" + xlated_num + "','" + time +"'";
CA::DBMAP dbsms_campaign;
if(!dbInt){
        logerror(m_strCRF,"BSCharging::DB_esp_get_sms_campaign() DB Connection failure");
         return retCode::BS_DB_CONNECTION_FAILED;
    }
    retCode ret = executeQuery(dbInt,spName,spValue,dbsms_campaign);

    logdebug(m_strCRF,"BSCharging::%s() DataBase : %s,Query %s",__func__,getDataBaseName(dbInt->m_connectionString).c_str(),dbInt->getLastQuery().c_str());
    if(ret !=  retCode::BS_SUCCESS){
	logdebug(m_strCRF, "%sDB_esp_get_sms_campaign DB:%s, Query:%s - failed %s", APP, m_strAccountProfile.c_str(), dbInt->getLastQuery().c_str(), getLastKnownError().c_str());
        ClearDBData(dbsms_campaign);
        return ret;
	}	
    if(dbsms_campaign.size() == 0){
        setLastKnownError("No SQL Record Found");
        return retCode::BS_DB_NO_RECORD;
    }
    ClearDBData(dbsms_campaign);
    return ret;
} 


retCode BSCharging::DB_es1_get_activation(CA::DBInterface* dbInt,std::string telco_code, std::string cust_code, 
					int batch_code, int serial_code) {
logdebug(m_strCRF,"BSCharging::DB_es1_get_activation() CLI %s DDI %s",m_strCli.c_str(),m_strDdi.c_str());
std::string spName = "es1_get_activation";
std::string spValue  = " '" + telco_code +
		               "','" + cust_code + "','" + std::to_string(batch_code) + "','" + std::to_string(serial_code) +"'";
CA::DBMAP dbactivation;
if(!dbInt){
        logerror(m_strCRF,"BSCharging::DB_es1_get_activation() DB Connection failure");
         return retCode::BS_DB_CONNECTION_FAILED;
    }
    retCode ret = executeQuery(dbInt,spName,spValue,dbactivation);

    logdebug(m_strCRF,"BSCharging::%s() DataBase : %s,Query %s",__func__,getDataBaseName(dbInt->m_connectionString).c_str(),dbInt->getLastQuery().c_str());
    if(ret !=  retCode::BS_SUCCESS){
	logdebug(m_strCRF, "%sDB_es1_get_activation DB:%s, Query:%s - failed %s", APP, m_strAccountProfile.c_str(), dbInt->getLastQuery().c_str(), getLastKnownError().c_str());
        ClearDBData(dbactivation);
        return ret;
	}	
    if(dbactivation.size() == 0){
        setLastKnownError("No SQL Record Found");
        ClearDBData(dbactivation);
        return retCode::BS_DB_NO_RECORD;
    }
    getData("isfree",0,dbactivation,m_activation.isFree);
    ClearDBData(dbactivation);
    return ret;
}


retCode BSCharging::DB_esp_get_package_prefixdest_bydid(CA::DBInterface* dbInt,std::string telco_code, 
	std::string cust_code, int batch_code, int serial_code, int packid, 
	int prefixdestMode, std::string prefixdest, std::string cli, std::string &inxlatednum) {
logdebug(m_strCRF,"BSCharging::DB_esp_get_package_prefixdest_bydid() CLI %s DDI %s",m_strCli.c_str(),m_strDdi.c_str());
std::string spName = "esp_get_package_prefixdest_bydid";
std::string spValue  =  "'" + telco_code +
		               "','" + cust_code + "'," + std::to_string(batch_code) + "," + std::to_string(serial_code) + "," +
		               std::to_string(packid) + ",'"+ std::to_string(prefixdestMode) + "','" + prefixdest + "','" + 
		               cli + "','" + m_mnpDet.mnpNum + "'";
CA::DBMAP dbpackage_prefixdest_bydid;
if(!dbInt){
        logerror(m_strCRF,"BSCharging::DB_esp_get_package_prefixdest_bydid() DB Connection failure");
         return retCode::BS_DB_CONNECTION_FAILED;
    }
    retCode ret = executeQuery(dbInt,spName,spValue,dbpackage_prefixdest_bydid);
    logdebug(m_strCRF,"BSCharging::%s() DataBase : %s,Query %s",__func__,getDataBaseName(dbInt->m_connectionString).c_str(),dbInt->getLastQuery().c_str());
    if(ret !=  retCode::BS_SUCCESS){
		logdebug(m_strCRF, "%sDB_esp_get_package_prefixdest_bydid DB:%s, Query:%s - failed %s", APP, m_strAccountProfile.c_str(),  dbInt->getLastQuery().c_str(), getLastKnownError().c_str());
        ClearDBData(dbpackage_prefixdest_bydid);
        return ret;
	}	
    if(dbpackage_prefixdest_bydid.size() == 0){
        setLastKnownError("No SQL Record Found");
        return retCode::BS_DB_NO_RECORD;
    }
    
    getData("prefixdest",0,dbpackage_prefixdest_bydid,inxlatednum);
    ClearDBData(dbpackage_prefixdest_bydid);
    return ret;
}
retCode BSCharging::DB_customquery(CA::DBInterface* dbInt,std::string curr_code, std::string dbTable, 
				std::string selQuery) {
logdebug(m_strCRF,"BSCharging::DB_customquery() CLI %s DDI %s",m_strCli.c_str(),m_strDdi.c_str());
std::string spName = "SELECT * FROM ccy WHERE sitecode= '";
std::string spValue  = m_strSiteCode + "' AND currcode='" + curr_code + "'";
CA::DBMAP dbcustomquery;
if(!dbInt){
        logerror(m_strCRF,"BSCharging::DB_customquery() DB Connection failure");
         return retCode::BS_DB_CONNECTION_FAILED;
    }
    retCode ret = executeQuery(dbInt,spName,spValue,dbcustomquery);
    logdebug(m_strCRF,"BSCharging::%s() DataBase : %s,Query %s",__func__,getDataBaseName(dbInt->m_connectionString).c_str(),dbInt->getLastQuery().c_str());
    if(ret !=  retCode::BS_SUCCESS){
		logdebug(m_strCRF, "%sDB_customquery DB:%s, Query:%s - failed %s", APP, m_strAccountProfile.c_str(), dbInt->getLastQuery().c_str(), getLastKnownError().c_str());
        ClearDBData(dbcustomquery);
        return ret;
	}	
    if(dbcustomquery.size() == 0){
        setLastKnownError("No SQL Record Found");
        return retCode::BS_DB_NO_RECORD;
    }
    ClearDBData(dbcustomquery);
    return ret;
}


retCode BSCharging::DB_esp_get_packagelimit_v2(CA::DBInterface* dbInt,std::string cust_code,int batch_code,int serial_code,
				int limit_mode,int limit_delay,double limit_amount,int limit_minute,int limit_call){
logdebug(m_strCRF,"BSCharging::DB_esp_get_packagelimit_v2() CLI %s DDI %s",m_strCli.c_str(),m_strDdi.c_str());
std::string spName = "esp_get_packagelimit_v2";
std::string time = GetTime();
std::string spValue  =  " '"+ cust_code + "',"
											+ std::to_string(batch_code) + "," + std::to_string(serial_code) + ","
											+ "'" + time + "'," + std::to_string(limit_mode) + ","
											+ std::to_string(limit_delay) + "," + std::to_string(limit_amount) + ","
											+ std::to_string(limit_minute) + "," + std::to_string(limit_call);
CA::DBMAP dbpackagelimit_v2;
if(!dbInt){
        logerror(m_strCRF,"BSCharging::DB_esp_get_packagelimit_v2() DB Connection failure");
         return retCode::BS_DB_CONNECTION_FAILED;
    }
    retCode ret = executeQuery(dbInt,spName,spValue,dbpackagelimit_v2);
    logdebug(m_strCRF,"BSCharging::%s() DataBase : %s,Query %s",__func__,getDataBaseName(dbInt->m_connectionString).c_str(),dbInt->getLastQuery().c_str());
    if(ret !=  retCode::BS_SUCCESS){
    logdebug(m_strCRF, "%sDB_esp_get_packagelimit_v2 DB:%s, Query:%s - failed %s", APP, m_strAccountProfile.c_str(), dbInt->getLastQuery().c_str(), getLastKnownError().c_str());
        ClearDBData(dbpackagelimit_v2);
        return ret;
	}	
    if(dbpackagelimit_v2.size() == 0){
        setLastKnownError("No SQL Record Found");
        return retCode::BS_DB_NO_RECORD;
    }
    getData("avail_bal",0,dbpackagelimit_v2,m_packageLimitv2.avail_balance);
	getData("avail_min",0,dbpackagelimit_v2,m_packageLimitv2.avail_min);
	getData("avail_call",0,dbpackagelimit_v2,m_packageLimitv2.avail_call);
    ClearDBData(dbpackagelimit_v2);
    return ret;
}
retCode BSCharging::CheckPBXCall(CA::DBInterface* dbInt,BSParam *bsp)
{
    retCode ret = retCode::BS_SUCCESS;
    ret = DB_PBX_esp_es2_get_ani(dbInt,m_strSiteCode,m_strDdi,m_strCli);
    ret = DB_PBX_esp_es1_get_clicat(dbInt,m_strSiteCode,m_strTrunkName,m_strCli);
    return ret;
}



retCode BSCharging::DB_PBX_esp_es2_get_ani(CA::DBInterface* dbInt,std::string sitecode, std::string didnum, std::string cli)
{
    logdebug(m_strCRF,"BSCharging::DB_PBX_esp_es2_get_ani() CLI %s DDI %s",m_strCli.c_str(),m_strDdi.c_str());
    std::string spName = "es2_get_ani";
    std::string spValue  =  " '"+ sitecode + "'," + "'" + didnum + "'," + "'" + cli + "'";
    CA::DBMAP dpes2ani;
    if(!dbInt)
    {
        logerror(m_strCRF,"BSCharging::DB_PBX_esp_es2_get_ani() DB Connection failure");
         return retCode::BS_DB_CONNECTION_FAILED;
    }

    retCode ret = executeQuery(dbInt,spName,spValue,dpes2ani);
    logdebug(m_strCRF,"BSCharging::%s() DataBase : %s,Query %s",__func__,getDataBaseName(dbInt->m_connectionString).c_str(),dbInt->getLastQuery().c_str());
    if(ret !=  retCode::BS_SUCCESS){
    logdebug(m_strCRF, "%DB_PBX_esp_es2_get_ani DB:%s, Query:%s - failed %s", APP, m_strAccountProfile.c_str(), dbInt->getLastQuery().c_str(), getLastKnownError().c_str());
       ClearDBData(dpes2ani);
        return ret;
	}
    if(dpes2ani.size() == 0){
        setLastKnownError("No SQL Record Found");
        return retCode::BS_DB_NO_RECORD;
    }
    getData("isactive",0,dpes2ani,m_es2aniinfo.isactive);
    getData("feature1",0,dpes2ani,m_es2aniinfo.feature1);
    ClearDBData(dpes2ani);
    return ret;
}

retCode BSCharging::DB_PBX_esp_es1_get_clicat(CA::DBInterface* dbInt,std::string sitecode,std::string trunkName,std::string cli)
{
    logdebug(m_strCRF,"BSCharging::DB_PBX_esp_es1_get_clicat() CLI %s DDI %s",m_strCli.c_str(),m_strDdi.c_str());
    std::string spName = "es1_get_clicat";
    std::string spValue  =  " '"+ sitecode + "'," + "'" + trunkName + "'," + "'" + cli + "'";
    CA::DBMAP dbes1clicat;
    if(!dbInt)
    {
        logerror(m_strCRF,"BSCharging::DB_PBX_esp_es1_get_clicat() DB Connection failure");
         return retCode::BS_DB_CONNECTION_FAILED;
    }

    retCode ret = executeQuery(dbInt,spName,spValue,dbes1clicat);
    logdebug(m_strCRF,"BSCharging::%s() DataBase : %s,Query %s",__func__,getDataBaseName(dbInt->m_connectionString).c_str(),dbInt->getLastQuery().c_str());
    if(ret !=  retCode::BS_SUCCESS){
    logdebug(m_strCRF, "%DB_PBX_esp_es1_get_clicat DB:%s, Query:%s - failed %s", APP, m_strAccountProfile.c_str(), dbInt->getLastQuery().c_str(), getLastKnownError().c_str());
    ClearDBData(dbes1clicat);
        return ret;
	}
    if(dbes1clicat.size() == 0){
        setLastKnownError("No SQL Record Found");
        return retCode::BS_DB_NO_RECORD;
    }
    getData("surcharge",0,dbes1clicat,m_es1clicatinfo.surcharge);
    getData("category",0,dbes1clicat,m_es1clicatinfo.category);
    getData("charge",0,dbes1clicat,m_es1clicatinfo.charge);
    ClearDBData(dbes1clicat);
    return ret;
}

bool BSCharging::ClearMasterDBData()
{
    return ClearDBData(m_dbPackByMaster);
}
