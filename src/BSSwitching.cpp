#include "BSSwitching.h"
#include "BusinessServer.h"
#include "LogHandler.h"

#define APP "MSWITCH:"

BSSwitching::BSSwitching(BSParam* bsc, std::string profile)
{
    
    m_strCli = bsc->getCli();
    m_strDdi = bsc->getDdi();
    m_strTrunkName = bsc->getTrunkName();
    m_strInterTrunkgroup =  bsc->getInterTrunkgroup();
    m_strSwitchCode = bsc->getStrSwitchCode();
    m_strSiteCode = bsc->getSiteCode();
    m_stroutGoingGroup = "";
    m_strSwitchingProfile = profile;
    m_strSwitchingSitecode = "BM3";
    m_strAppID =  generateAppID();
    m_strCRF = bsc->getUid(); // call reference ID
    m_strCliAdd = bsc->getCliAddr();
    m_strDdiAdd = bsc->getDdiAddr();
    m_strTrace = bsc->getTrace();
    setBsRequestTime();
    #if 0
    std::string taskName = bsc->m_taskName->GetValueByPosition(bsc->m_currentAppPos);
    SPAWNTASKINFO* sTask =  bsc->m_spawnTaskInfo->FindValue(taskName);
    if(sTask){
        m_strSwitchingProfile = sTask->switchdb;
    }
    #endif
}


BSSwitching::~BSSwitching(){
 
}
retCode BSSwitching::InitiateSwitching(CA::CAMap<CA::DBInterface*>& dblist, BSParam* bsp)
{
    logdebug(m_strCRF,"BSSwitching::InitiateSwitching()");
    if (dblist.GetMapSize()  == 0){
        setLastKnownError("DB Connection list empty");
        return retCode::BS_ERROR_UNKOWN;
    }

    if(bsp == nullptr){
        setLastKnownError("CallIn param object is NULL");
        return retCode::BS_ERROR_UNKOWN;
    }

    CA::DBInterface* dbInt = dblist.FindValue(m_strSwitchingProfile); // Loading Main Account Profile

    logdebug(m_strCRF, "%sInitiateSwitching DID : %s, CLI : %s, TRUNK : %s, INTER TRUNK : %s", \
                    APP, m_strDdi.c_str(), m_strCli.c_str(), m_strTrunkName.c_str(), m_strInterTrunkgroup.c_str());
    //DB_get_outTrunkInterface(dbInt,m_strSwitchingSitecode,m_stroutGoingGroup);
    retCode ret;
    ret = DB_get_outTrunkInterface(dbInt,m_strSwitchingSitecode,m_strInterTrunkgroup);
    //if(DB_es5_get_config(dbInt,m_stroutGoingGroup, m_strDdi) != retCode::BS_SUCCESS)
    m_stroutGoingGroup = "rtp1";
    if((ret = DB_es5_xlat_callout(dbInt,m_strSwitchingSitecode,m_strSwitchCode,m_strDdi,m_strInterTrunkgroup,m_stroutGoingGroup)) != retCode::BS_SUCCESS)
    {
	    logerror(m_strCRF,"BSRouting::InitiateSwitchinging DB_es5_xlat_callout - %s",getLastKnownError().c_str());
        return ret;
    }
    

     if((ret = DB_es5_get_config(dbInt,m_strInterTrunkgroup, m_strDdi)) != retCode::BS_SUCCESS)
    {
	    logerror(m_strCRF,"BSSwitching::InitiateSwitchinging DB_es5_get_config - %s",getLastKnownError().c_str());
       // return false;
    }
    xlatnb = ProcPrefix(m_dbes5XlatCallout.xlatpfx,m_dbes5XlatCallout.prefix,m_strDdi).c_str();
    int cliNumType = 1;

    if((ret = DB_cs_get_xlatout(dbInt,m_strSwitchingSitecode,m_strSwitchCode,m_strInterTrunkgroup, m_strCli, cliNumType,1)) != retCode::BS_SUCCESS)
    {
	    logerror(m_strCRF,"BSSwitching::InitiateSwitchinging DB_cs_get_xlatout - %s",getLastKnownError().c_str());
        
    }
    m_strDdi = xlatnb;
    m_strDdiAdd = m_dbes5XlatCallout.signaddress;
    bsp->setDdi(m_strDdi);
    bsp->setDdiAddr(m_strDdiAdd);
    bsp->setCli(m_strCli);
    if(m_dbes5XlatCallout.signaddress.empty()){
        logdebug(m_strCRF, "BSSwitching::InitiateSwitchinging Signal Address in Empty");
        return retCode::BS_NO_SIGNALIP;
    }
    logdebug(m_strCRF,"BSSwitching Details = m_strCli-%s,m_strDdi-%s,m_strInterTrunkgroup-%s,m_strSwitchCode-%s,m_strSiteCode-%s,m_stroutGoingGroup-%s,m_strSwitchingProfile-%s,m_strSwitchingSitecode-%s,m_stroutGoingGroup-%s,m_strCRF-%s,m_strCliAdd-%s,m_strDdiAdd-%s",m_strCli.c_str(),m_strDdi.c_str(),m_strInterTrunkgroup.c_str(),m_strSwitchCode.c_str(),m_strSiteCode.c_str(),m_stroutGoingGroup.c_str(),m_strSwitchingProfile.c_str(),m_strSwitchingSitecode.c_str(),m_stroutGoingGroup.c_str(),m_strCRF.c_str(),m_strCliAdd.c_str(),m_strDdiAdd.c_str());
    return retCode::BS_SUCCESS;
}

retCode BSSwitching::DB_get_outTrunkInterface(CA::DBInterface* dbInt,std::string sitecode, std::string group){
    //std::string spName =  "get_outTrunk_Interface";
    std::string spName =  "mswicth_getRLsInterface";
    std::string spValue = "'" + group + "'";
    if(!dbInt)
	{
        logerror(m_strCRF,"BSSwitching::DB_get_outTrunkInterface() DB Connection failure");
        return retCode::BS_DB_CONNECTION_FAILED;
    }
    CA::DBMAP dboutTrnkInterface;
    retCode ret = executeQuery(dbInt,spName,spValue,dboutTrnkInterface);
    logdebug(m_strCRF,"BSSwitching::%s() DataBase : %s,Query %s",__func__,getDataBaseName(dbInt->m_connectionString).c_str(),dbInt->getLastQuery().c_str());
    if(ret != retCode::BS_SUCCESS){
        logerror(m_strCRF, "%sDB_get_outTrunkInterface DB:%s, Query:%s - failed %s", APP, m_strSwitchingProfile.c_str(), dbInt->getLastQuery().c_str(), getLastKnownError().c_str());
        ClearDBData(dboutTrnkInterface);
        return ret;
    }

    if(dboutTrnkInterface.size() == 0){
        setLastKnownError("No SQL Record Found");
        return retCode::BS_DB_NO_RECORD;
    }
    getData("interface",0,dboutTrnkInterface,m_stroutGoingGroup);
    ClearDBData(dboutTrnkInterface);
    return ret;
}


retCode BSSwitching::DB_es5_xlat_callout(CA::DBInterface* dbInt,std::string sitecode, std::string switchcode,
        std::string didnum, std::string trunk, std::string group){
    std::string spName =  "es5_xlat_callout";
    std::string spValue = " '" + sitecode + "','" +
                               switchcode + "','" + didnum + "','" + trunk + "','" + trunk + "','" + group + "'";
    if(!dbInt)
	{
        logerror(m_strCRF,"BSSwitching::DB_es5_xlat_callout() DB Connection failure");
        return retCode::BS_DB_CONNECTION_FAILED;
    }
    CA::DBMAP dbEs5XlatCallout;
    retCode ret = executeQuery(dbInt,spName,spValue,dbEs5XlatCallout);
    logdebug(m_strCRF,"BSSwitching::%s() DataBase : %s,Query %s",__func__,getDataBaseName(dbInt->m_connectionString).c_str(),dbInt->getLastQuery().c_str());
    if(ret != retCode::BS_SUCCESS){
        logdebug(m_strCRF, "%sDB_es5_xlat_callout DB:%s, Query:%s - failed %s", APP, m_strSwitchingProfile.c_str(), dbInt->getLastQuery().c_str(), getLastKnownError().c_str());
        ClearDBData(dbEs5XlatCallout);
        return ret;
    }

    if(dbEs5XlatCallout.size() == 0){
        setLastKnownError("No SQL Record Found");
        return retCode::BS_DB_NO_RECORD;
    }
    getData("xlatpfx",0,dbEs5XlatCallout,m_dbes5XlatCallout.xlatpfx);
    getData("prefix",0,dbEs5XlatCallout,m_dbes5XlatCallout.prefix);
    getData("numbertype",0,dbEs5XlatCallout,m_dbes5XlatCallout.numbertype);
    getData("complaw",0,dbEs5XlatCallout,m_dbes5XlatCallout.complaw);
    getData("nbplan",0,dbEs5XlatCallout,m_dbes5XlatCallout.nbplan);
    getData("signaddress",0,dbEs5XlatCallout,m_dbes5XlatCallout.signaddress);
    getData("callingnb",0,dbEs5XlatCallout,m_dbes5XlatCallout.callingnb);
    getData("callingcat",0,dbEs5XlatCallout,m_dbes5XlatCallout.callingcat);
    getData("callingnbtype",0,dbEs5XlatCallout,m_dbes5XlatCallout.callingnbtype);
    getData("did_len",0,dbEs5XlatCallout,m_dbes5XlatCallout.didlen);
    getData("callingscreening",0,dbEs5XlatCallout,m_dbes5XlatCallout.callingscreening);
    getData("presentation",0,dbEs5XlatCallout,m_dbes5XlatCallout.presentation);
    getData("mindidlen",0,dbEs5XlatCallout,m_dbes5XlatCallout.mindidlen);
    getData("conntype",0,dbEs5XlatCallout,m_dbes5XlatCallout.conntype);
    ClearDBData(dbEs5XlatCallout);
    return ret;
}

retCode BSSwitching::DB_es5_get_config(CA::DBInterface* dbInt,std::string trunk, std::string didnum){
    std::string spName =  "es5_get_config";
    std::string spValue = " '" + trunk + "','" + didnum + "'";
    if(!dbInt)
	{
        logerror(m_strCRF,"BSSwitching::DB_es5_get_config() DB Connection failure");
        return retCode::BS_DB_CONNECTION_FAILED;
    }
    CA::DBMAP dbes5Config;
    retCode ret = executeQuery(dbInt,spName,spValue,dbes5Config);
    logdebug(m_strCRF,"BSSwitching::%s() DataBase : %s,Query %s",__func__,getDataBaseName(dbInt->m_connectionString).c_str(),dbInt->getLastQuery().c_str());
    if(ret != retCode::BS_SUCCESS){
        logdebug(m_strCRF, "%sDB_es5_get_config DB:%s, Query:%s - failed %s", APP, m_strSwitchingProfile.c_str(), dbInt->getLastQuery().c_str(), getLastKnownError().c_str());
        ClearDBData(dbes5Config);
        return ret;
    }

    if(dbes5Config.size() == 0){
        setLastKnownError("No SQL Record Found");
        return retCode::BS_DB_NO_RECORD;
    }
    ClearDBData(dbes5Config);
    return ret;
}


retCode BSSwitching::DB_cs_get_xlatout(CA::DBInterface* dbInt,std::string sitecode, std::string switchcode,
        std::string trunk, std::string &cli, int num1, int num2){
    std::string spName =  "cs_get_xlatout";
    std::string spValue = " '" + sitecode + "','" +
                        switchcode + "','" + trunk + "','" + cli + "'," + to_string(num1) + "," +
                        to_string(num2);
    if(!dbInt)
	{
        logerror(m_strCRF,"BSSwitching::DB_cs_get_xlatout() DB Connection failure");
        return retCode::BS_DB_CONNECTION_FAILED;
    }
    CA::DBMAP dbcsXlatout;
    retCode ret = executeQuery(dbInt,spName,spValue,dbcsXlatout);
    logdebug(m_strCRF,"BSSwitching::%s() DataBase : %s,Query %s",__func__,getDataBaseName(dbInt->m_connectionString).c_str(),dbInt->getLastQuery().c_str());
    if(ret != retCode::BS_SUCCESS){
        logdebug(m_strCRF, "%sDB_cs_get_xlatout DB:%s, Query:%s - failed %s", APP, m_strSwitchingProfile.c_str(), dbInt->getLastQuery().c_str(), getLastKnownError().c_str());
        ClearDBData(dbcsXlatout);
        return ret;
    }

    if(dbcsXlatout.size() == 0){
        setLastKnownError("No SQL Record Found");
        return retCode::BS_DB_NO_RECORD;
    }
    std::string xlatpfx;
    std::string prefix;
    getData("xlatpfx",0,dbcsXlatout,xlatpfx);
    getData("prefix",0,dbcsXlatout,prefix);
    m_strCli = ProcPrefix(xlatpfx,prefix,m_strCli).c_str();
    ClearDBData(dbcsXlatout);
    return ret;
}

std::string BSSwitching::ProcPrefix(std::string newprefix,std::string prefix, std::string nb){
	std::string ret  = "";
	std::string tempPrefix= "";
    // Need to check the below trim function
    #if 0
	trim(newprefix);
	trim(prefix);
	trim(nb);

    #endif

	std::size_t postion = nb.find(prefix);
	std::size_t ppref_pos = prefix.find("_");
	if(prefix.empty() || (postion  == std::string::npos && ppref_pos == std::string::npos)  || prefix.find("*") != std::string::npos)
	   tempPrefix = nb;
	else	{
		tempPrefix = nb.substr(prefix.length(),(nb.length()-prefix.length()));
		
	}
	ret = newprefix + tempPrefix;
	
return ret;
}

