#include "BSRouting.h"
#include "LogHandler.h"
#include "BusinessServer.h"
#define APP "MROUTE:"

BSRouting::BSRouting(BSParam* bsp ,Application app)
{

        //m_strSiteCode =  bsp->getSiteCode();
        m_strSiteCode = "BM3"; // hardcoded for testing
        m_strCli =  bsp->getCli();
        m_strDdi =  bsp->getDdi();
        m_RouteCount = 0;
        m_strTrunkName = bsp->getTrunkName();
        m_strInterTrunkgroup =  bsp->getInterTrunkgroup();
        m_strSwitchCode =  bsp->getStrSwitchCode();
        m_strCRF = bsp->getUid();
        m_strRoutingProfile =  app.getRoutingDB();
        m_strSwitchingProfile = app.getSwitchDB();
        m_strAppID =  generateAppID();
        setBsRequestTime();
        //bsp->setSwitchProfile(app.getSwitchDB()); // = app.getSwitchDB();
        m_strGradeCode = "L0110";
        m_strTrace = bsp->getTrace();
        }

BSRouting::~BSRouting() {

}
retCode BSRouting::GetAlternateRoute(CA::CAMap<CA::DBInterface*>& dblist, BSParam* bsp)
{
    logdebug(m_strCRF,"BSRouting::GetAlternateRoute() ==> RoutingProfile : %s && ddi:%s", m_strRoutingProfile.c_str(), m_strDdi.c_str());
    if (dblist.GetMapSize()  == 0){
        setLastKnownError("DB Connection list empty");
        return retCode::BS_ERROR_UNKOWN;
    }

    if(bsp == nullptr){
        setLastKnownError("CallIn param object is NULL");
        return retCode::BS_NULL_OBJECT;
    }

    std::cout<<"Mroute - DB profile -- "<<m_strRoutingProfile <<std::endl;

    CA::DBInterface* dbInt = dblist.FindValue(m_strRoutingProfile); // Loading Main Account Profile

    if(!dbInt){
        return retCode::BS_DB_CONNECTION_FAILED;
    }
    retCode ret = retCode::BS_SUCCESS;
    float rCost = 0.0;
        int minRoute = 15;
    if(0 >= validRoute.size())
    {
        // here need set proper cause code and call stete for disconnect the call
        logerror(m_strCRF,"BSRouting::GetAlternateRoute No Valid route" )   ;
        return retCode::BS_NO_ROUTE;
    }
    m_RouteCount++;
    logdebug(m_strCRF,"BSRouting::GetAlternateRoute() ==> m_RouteCount : %d validRoute.size():%d", m_RouteCount,validRoute.size());
    if(m_RouteCount < validRoute.size())
    {
        std::string trunckcode = validRoute[m_RouteCount]->group.c_str();
        m_strInterTrunkgroup = trunckcode.c_str();
        bsp->setcallCost(validRoute[m_RouteCount]->cost);
	bsp->setInterTrunkGroup(validRoute[m_RouteCount]->group);
	bsp->setDdi(m_strDdi.c_str());
	ret = DirectSwitching(dblist,bsp,0);
    }
    else
    {
        logdebug(m_strCRF,"%sGetAlternateRoute tried all available routes", APP);
        return retCode::BS_NO_ROUTE;
    }
    return ret;
}


retCode BSRouting::InitiateRouting(CA::CAMap<CA::DBInterface*>& dblist, BSParam* bsp)
{
    logdebug(m_strCRF,"BSRouting::InitiateRouting()");
    if (dblist.GetMapSize()  == 0){
        setLastKnownError("DB Connection list empty");
        return retCode::BS_ERROR_UNKOWN;
    }

    if(bsp == nullptr){
        setLastKnownError("CallIn param object is NULL");
        return retCode::BS_NULL_OBJECT;
    }

    std::cout<<"Mroute - DB profile -- "<<m_strRoutingProfile <<std::endl;

    CA::DBInterface* dbInt = dblist.FindValue(m_strRoutingProfile); // Loading Main Account Profile

    if(!dbInt){
        return retCode::BS_DB_CONNECTION_FAILED;
    }
    retCode ret = retCode::BS_SUCCESS;
    DB_es3_getxlat(dbInt,m_strSwitchCode,m_strInterTrunkgroup,m_strDdi);
    if(( ret = DB_sl_checkddiblocked(dbInt,m_strDdi)) !=  retCode::BS_SUCCESS)
    {
      logerror(m_strCRF,"BSRouting::InitiateRouting DB_sl_checkddiblocked - %s",getLastKnownError().c_str())   ;
    }
    if(( ret = DB_r01_get_prefix_ref(dbInt,m_strDdi)) !=  retCode::BS_SUCCESS)
    {
        logerror(m_strCRF,"BSRouting::InitiateRouting DB_r01_get_prefix_ref - %s",getLastKnownError().c_str())   ;
    }
    int operatorSet = 0; // Now hardcoded temporarly
    if(( ret = DB_cr_get_operatorset(dbInt,operatorSet)) !=  retCode::BS_SUCCESS)
    {
        logerror(m_strCRF,"BSRouting::InitiateRouting DB_cr_get_operatorset - %s",getLastKnownError().c_str())   ; 
    }
    // below four values also need to verify with Yadav
    std::string tmp1 ="";
	std::string tmp2 = "";
    int num1 = 0;
    int num2 = 0;
    if((ret = DB_r25_get_india_did_route(dbInt,m_strCli)) == retCode::BS_SUCCESS)
    {
	logdebug(m_strCRF,"BSRouting::InitiateRouting DB_r25_get_india_did_route -SUCCESS");   
    }
    else if((ret = DB_r25_get_route(dbInt,m_strDdi,m_strInterTrunkgroup,tmp1,m_strSiteCode,num1,tmp2,num2)) != retCode::BS_SUCCESS)
    {
        logerror(m_strCRF,"BSRouting::InitiateRouting DB_r25_get_route - %s",getLastKnownError().c_str());
        //return ret;
    }
    if((ret = DB_esp_get_routelimit(dbInt,m_strSiteCode,m_strDdi)) != retCode::BS_SUCCESS)
    {
        logerror(m_strCRF,"BSRouting::InitiateRouting DB_esp_get_routelimit - %s",getLastKnownError().c_str())   ;
        //return ret;
    }
    // check the below values and check the vector is filled or not in DB_r25_get_route function
    float rCost = 0.0;
	int minRoute = 15;
    if(0 >= validRoute.size())
    {
        // here need set proper cause code and call stete for disconnect the call
        logerror(m_strCRF,"BSRouting::InitiateRouting No Valid route" )   ;
        return retCode::BS_NO_ROUTE;
    }
    std::string trunckcode = validRoute[m_RouteCount]->group.c_str();
         m_strInterTrunkgroup = trunckcode.c_str();
         bsp->setcallCost(validRoute[m_RouteCount]->cost);
    if((ret = DB_mroute_get_tdm_ip_address(dbInt,m_strSiteCode,m_strSwitchCode,m_strInterTrunkgroup,m_strDdi)) != retCode::BS_SUCCESS)
    {
        // here we are calling direct switching
        #if 0
        std::cout<<"DB_mroute_get_tdm_ip_address -- if case"<<std::endl;
        std::unique_ptr<BSSwitching> switch_ptr (new BSSwitching(bsp));
        switch_ptr->InitiateSwitching(dblist,bsp);
        m_switchPtr = std::move(switch_ptr);
        #endif
        bsp->setInterTrunkGroup(validRoute[m_RouteCount]->group); // first route switching details group
       ret = DirectSwitching(dblist,bsp,0);
        


    }else{
        // here we are getting windows ip and bypass the Mswitch
        bsp->setDdiAddr(m_tdmipaddress.ipaddress);

        setUserInfo(bsp);
        //setPrivateInfo(bsp);
        bsp->setPinfo("cdsbwdXs22jJZ4171303202315192800001:127.0.0.1:rtp9");

        return retCode::BS_SUCCESS;
    }
    return ret;
}



retCode BSRouting::DB_es3_getxlat(CA::DBInterface* dbInt,std::string sitecode, std::string trunk, std::string &didnum){
    std::string spName =  "es3_get_xlat";
    std::string spValue = " '"+sitecode+"','"+didnum+"','"+trunk+"',"+"1"+",'"+m_strSwitchCode+"'";
    if(!dbInt)
	{
        logerror(m_strCRF,"BSRouting::DB_es3_getxlat() DB Connection failure");
        return retCode::BS_DB_CONNECTION_FAILED;
    }
    CA::DBMAP dbxlates3;
    retCode ret = executeQuery(dbInt,spName,spValue,dbxlates3);
    logdebug(m_strCRF,"BSRouting::%s() DataBase : %s,Query %s",__func__,getDataBaseName(dbInt->m_connectionString).c_str(),dbInt->getLastQuery().c_str());
    if(ret != retCode::BS_SUCCESS){
        logdebug(m_strCRF, "%sDB_es3_getxlat DB:%s, Query:%s - failed %s", APP, m_strRoutingProfile.c_str(), dbInt->getLastQuery().c_str(), getLastKnownError().c_str());
        setLastKnownError(dbInt->getLastKnownError());
        ClearDBData(dbxlates3);
        return ret;
    }

    if(dbxlates3.size() == 0){
        setLastKnownError("No SQL Record Found");
        return retCode::BS_DB_NO_RECORD;
    }
    getData("xlatpfx",0,dbxlates3,m_es3xlat.xlatpfx);
    getData("prefix",0,dbxlates3,m_es3xlat.prefix);
    std::string xlatout =  ProcPrefix(m_es3xlat.xlatpfx,m_es3xlat.prefix,didnum).c_str();
    if(xlatout.empty() != true)
        didnum = xlatout;
    ClearDBData(dbxlates3); 
    return ret;
}

retCode BSRouting::DB_sl_checkddiblocked(CA::DBInterface* dbInt,std::string didnum){
    std::string spName =  "sl_checkddiblocked";
    std::string spValue =  " '" + didnum + "'";
    if(!dbInt)
	{
        logerror(m_strCRF,"BSRouting::DB_sl_checkddiblocked() DB Connection failure");
        return retCode::BS_DB_CONNECTION_FAILED;
    }
    CA::DBMAP dbCheckDDIBlocked;
    retCode ret = executeQuery(dbInt,spName,spValue,dbCheckDDIBlocked);
    logdebug(m_strCRF,"BSRouting::%s() DataBase : %s,Query %s",__func__,getDataBaseName(dbInt->m_connectionString).c_str(),dbInt->getLastQuery().c_str());
    if(ret != retCode::BS_SUCCESS){
        logdebug(m_strCRF, "%sDB_sl_checkddiblocked DB:%s, Query:%s - failed %s", APP, m_strRoutingProfile.c_str(), dbInt->getLastQuery().c_str(), getLastKnownError().c_str());
        setLastKnownError(dbInt->getLastKnownError());
        ClearDBData(dbCheckDDIBlocked);
        return ret;
    }

    if(dbCheckDDIBlocked.size() == 0){
        setLastKnownError("No SQL Record Found");
        return retCode::BS_DB_NO_RECORD;
    }
    ClearDBData(dbCheckDDIBlocked);
    return ret;
}

retCode BSRouting::DB_r01_get_prefix_ref(CA::DBInterface* dbInt,std::string didnum){
    std::string spName =  "r01_get_prefix_ref";
    std::string spValue = " '" + didnum + "'";
    if(!dbInt)
	{
        logerror(m_strCRF,"BSRouting::DB_r01_get_prefix_ref() DB Connection failure");
        return retCode::BS_DB_CONNECTION_FAILED;
    }
    CA::DBMAP dbR01GetPrefixRef;
    retCode ret = executeQuery(dbInt,spName,spValue,dbR01GetPrefixRef);
    logdebug(m_strCRF,"BSRouting::%s() DataBase : %s,Query %s",__func__,getDataBaseName(dbInt->m_connectionString).c_str(),dbInt->getLastQuery().c_str());
    if(ret != retCode::BS_SUCCESS){
        logerror(m_strCRF, "%sDB_r01_get_prefix_ref DB:%s, Query:%s - failed %s", APP, m_strRoutingProfile.c_str(), dbInt->getLastQuery().c_str(), getLastKnownError().c_str());
        setLastKnownError(dbInt->getLastKnownError());
        ClearDBData(dbR01GetPrefixRef);
        return ret;
    }

    if(dbR01GetPrefixRef.size() == 0){
        setLastKnownError("No SQL Record Found");
        return retCode::BS_DB_NO_RECORD;
    }
    getData("prefix_ref",0,dbR01GetPrefixRef,m_prefixRefData.prefixRef);
    ClearDBData(dbR01GetPrefixRef);
    return ret;
}

retCode BSRouting::DB_cr_get_operatorset(CA::DBInterface* dbInt,int num){
    std::string spName =  "cr_get_operatorset";
    std::string spValue = "'0'";
    if(!dbInt)
	{
        logerror(m_strCRF,"BSRouting::DB_cr_get_operatorset() DB Connection failure");
        return retCode::BS_DB_CONNECTION_FAILED;
    }
    CA::DBMAP dbCrGetOperatorset;
    retCode ret = executeQuery(dbInt,spName,spValue,dbCrGetOperatorset);
    logdebug(m_strCRF,"BSRouting::%s() DataBase : %s,Query %s",__func__,getDataBaseName(dbInt->m_connectionString).c_str(),dbInt->getLastQuery().c_str());
    if(ret != retCode::BS_SUCCESS){
        logerror(m_strCRF, "%sDB_cr_get_operatorset DB:%s, Query:%s - failed %s", APP, m_strRoutingProfile.c_str(), dbInt->getLastQuery().c_str(), getLastKnownError().c_str());
        setLastKnownError(dbInt->getLastKnownError());
        ClearDBData(dbCrGetOperatorset);
        //return ret;
    }

    if(dbCrGetOperatorset.size() == 0){
        setLastKnownError("No SQL Record Found");
        //return retCode::BS_DB_NO_RECORD;
    }
    ClearDBData(dbCrGetOperatorset);
    return ret;
}


retCode BSRouting::DB_r25_get_india_did_route(CA::DBInterface* dbInt,std::string mnpNum)
{
	logdebug(m_strCRF,"BSRouting::DB_r25_get_india_did_route Entry()");
	std::string spName =  "r25_get_india_did_route";
	CA::DBMAP dbR25GetRoute;
	std::string spValue = " '" + mnpNum + "'";
	if(!dbInt)
	{
		logerror(m_strCRF,"BSRouting::DB_r25_get_india_did_route() DB Connection failure");
		return retCode::BS_DB_CONNECTION_FAILED;
	}
	retCode ret = executeQuery(dbInt,spName,spValue,dbR25GetRoute);
        logdebug(m_strCRF,"BSRouting::%s() DataBase : %s,Query %s",__func__,getDataBaseName(dbInt->m_connectionString).c_str(),dbInt->getLastQuery().c_str());
	if(dbR25GetRoute.size() == 0){
		setLastKnownError("No SQL Record Found");
		logerror(m_strCRF,"BSRouting::InitiateRouting DB_r25_get_india_did_route() - %s",getLastKnownError().c_str());
		return retCode::BS_DB_NO_RECORD;
	}
	validRoute.clear();
	R25GetRoute* m_r25GetRoute = new R25GetRoute();
	int i = 0 ;
	getData("group",i,dbR25GetRoute,m_r25GetRoute->group);
	getData("cost",i,dbR25GetRoute,m_r25GetRoute->cost);
	validRoute.push_back(m_r25GetRoute);
	logdebug(m_strCRF,"BSRouting::DB_r25_get_india_did_route() group %s, cost %f", m_r25GetRoute->group.c_str(), m_r25GetRoute->cost);
        ClearDBData(dbR25GetRoute);
	return retCode::BS_SUCCESS;
}
/* Need to implement full logic*/

retCode BSRouting::DB_r25_get_route(CA::DBInterface* dbInt,std::string mnpNum, std::string group,
        std::string tmp1, std::string sitecode, int num1, std::string tmp2, int num2){
    std::string spName =  "r25_get_route";
    CA::DBMAP dbR25GetRoute;
    std::string time = GetTime();

    std::string spValue = " '" + mnpNum + "'" +
                ",'" + time + "'" + ",'" + group + "'" + ",'" + tmp1 + "'" + ",'" + sitecode + "'" +
                "," + to_string(num1) + ",'" + tmp2 + "'," + to_string(num2) + "," + to_string(num2); // passed extra parameter num2 for Mysql default parameter kiran;
    if(!dbInt)
	{
        logerror(m_strCRF,"BSRouting::DB_r25_get_route() DB Connection failure");
        return retCode::BS_DB_CONNECTION_FAILED;
    }
    retCode ret = executeQuery(dbInt,spName,spValue,dbR25GetRoute);
    logdebug(m_strCRF,"BSRouting::%s() DataBase : %s,Query %s",__func__,getDataBaseName(dbInt->m_connectionString).c_str(),dbInt->getLastQuery().c_str());
    if(ret != retCode::BS_SUCCESS){
        logdebug(m_strCRF, "%sDB_r25_get_route DB:%s, Query:%s - failed %s", APP, m_strRoutingProfile.c_str(), dbInt->getLastQuery().c_str(), getLastKnownError().c_str());
        setLastKnownError(dbInt->getLastKnownError());
        ClearDBData(dbR25GetRoute);
        return ret;
    }

    if(dbR25GetRoute.size() == 0){
        setLastKnownError("No SQL Record Found");
        return retCode::BS_DB_NO_RECORD;
    }

    validRoute.clear();
    int sel = 0;
    //for (std::map<CA::DBMAP>::iterator it = dbR25GetRoute.begin() ; it != dbR25GetRoute.end(); it++)
    //{
        for(int i =0; i < dbR25GetRoute.size(); i++)
    {
		R25GetRoute* m_r25GetRoute = new R25GetRoute();
        getData("routeset",i,dbR25GetRoute,m_r25GetRoute->routeset);
        getData("priority",i,dbR25GetRoute,m_r25GetRoute->priority);
        getData("id",i,dbR25GetRoute,m_r25GetRoute->id);
        getData("isactive",i,dbR25GetRoute,m_r25GetRoute->isactive);
        getData("reason",i,dbR25GetRoute,m_r25GetRoute->reason);
        getData("exception",i,dbR25GetRoute,m_r25GetRoute->exception);
        getData("ext",i,dbR25GetRoute,m_r25GetRoute->ext);
        getData("routecls",i,dbR25GetRoute,m_r25GetRoute->routecls);
        getData("prefixcode",i,dbR25GetRoute,m_r25GetRoute->prefixcode);
        getData("universe",i,dbR25GetRoute,m_r25GetRoute->universe);
        getData("domain",i,dbR25GetRoute,m_r25GetRoute->domain);
        getData("pdomain",i,dbR25GetRoute,m_r25GetRoute->pdomain);
        getData("group",i,dbR25GetRoute,m_r25GetRoute->group);
        getData("interface",i,dbR25GetRoute,m_r25GetRoute->interface);
        getData("userinfo",i,dbR25GetRoute,m_r25GetRoute->userinfo);
        getData("hint",i,dbR25GetRoute,m_r25GetRoute->hint);
        getData("clsOrg",i,dbR25GetRoute,m_r25GetRoute->clsOrg);
        getData("timecls",i,dbR25GetRoute,m_r25GetRoute->timecls);
        getData("cost",i,dbR25GetRoute,m_r25GetRoute->cost);
        getData("flag",i,dbR25GetRoute,m_r25GetRoute->flag);
        getData("rating",i,dbR25GetRoute,m_r25GetRoute->rating);
        getData("access",i,dbR25GetRoute,m_r25GetRoute->access);
        getData("redlist",i,dbR25GetRoute,m_r25GetRoute->redlist);
        getData("grade",i,dbR25GetRoute,m_r25GetRoute->grade);
        getData("intcls",i,dbR25GetRoute,m_r25GetRoute->intcls);
        getData("capability",i,dbR25GetRoute,m_r25GetRoute->capability);
        getData("oprtype",i,dbR25GetRoute,m_r25GetRoute->oprtype);
        getData("costprice",i,dbR25GetRoute,m_r25GetRoute->costprice);

        int bestRoute = 0;//;std::stoi(strBestRoute);
		int worstRoute = 90;
        if(!m_strGradeCode.empty())
        {
            std::string strBestRoute = m_strGradeCode.substr(2-1, 2);
            if(!strBestRoute.empty())
                bestRoute = std::stoi(strBestRoute);
            std::string strWorstRoute = m_strGradeCode.substr(4-1, 2);
            if(!strWorstRoute.empty())
                worstRoute = std::stoi(strWorstRoute);
		}
        logdebug(m_strCRF, "BSRouting::DB_r25_get_route() best: %d - Worst: %d", bestRoute,worstRoute );
        if ( ((int)m_r25GetRoute->grade) >= bestRoute && ((int)m_r25GetRoute->grade)<= worstRoute) 
        {
            sel++;
            logdebug(m_strCRF, "BSRouting::DB_r25_get_route() %d: %s@%s .Cost:%3.4f Access: %s Flag: %d", sel, m_r25GetRoute->group.c_str(), m_r25GetRoute->domain.c_str(), \
                    m_r25GetRoute->cost, m_r25GetRoute->access.c_str(), m_r25GetRoute->flag);
	    double m_costprice = m_r25GetRoute->costprice;
            validRoute.push_back(m_r25GetRoute);
        }else{
            // print error This opr not in the allowed grade
            logdebug(m_strCRF,"%sDB_r25_get_route This opr not in the allowed grade. %s@%s", APP, \
                m_r25GetRoute->group.c_str(), m_r25GetRoute->domain.c_str());
            delete m_r25GetRoute;
            m_r25GetRoute = nullptr;
        }
    }
       ClearDBData(dbR25GetRoute);
	return ret;
}


std::string BSRouting::ProcPrefix(std::string newprefix,std::string prefix, std::string nb){
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

retCode BSRouting::DB_esp_get_routelimit(CA::DBInterface* dbInt,std::string sitecode, std::string mnpNum){
    std::string spName =  "esp_get_routelimit";
    std::string spValue = " '" + sitecode + "'" + ",'" + mnpNum + "'";
    if(!dbInt)
	{
        logerror(m_strCRF,"BSRouting::DB_esp_get_routelimit() DB Connection failure");
        return retCode::BS_DB_CONNECTION_FAILED;
    }
    CA::DBMAP dbEspGetRoutelimit;
    retCode ret = executeQuery(dbInt,spName,spValue,dbEspGetRoutelimit);
    logdebug(m_strCRF,"BSRouting::%s() DataBase : %s,Query %s",__func__,getDataBaseName(dbInt->m_connectionString).c_str(),dbInt->getLastQuery().c_str());
    if(ret != retCode::BS_SUCCESS){
        logdebug(m_strCRF, "%sDB_esp_get_routelimit DB:%s, Query:%s - failed %s", APP, m_strRoutingProfile.c_str(), dbInt->getLastQuery().c_str(), getLastKnownError().c_str());
        setLastKnownError(dbInt->getLastKnownError());
        ClearDBData(dbEspGetRoutelimit);
        return ret;
    }

    if(dbEspGetRoutelimit.size() == 0){
        setLastKnownError("No SQL Record Found");
        return retCode::BS_DB_NO_RECORD;
    }
    ClearDBData(dbEspGetRoutelimit);
    return ret;
}

retCode BSRouting::DB_mroute_get_tdm_ip_address(CA::DBInterface* dbInt,std::string sitecode,std::string switchcode,std::string trunckcode,std::string mnpNum){
    std::string spName =  "mroute_get_tdm_ip_address";
    std::string spValue =  " '" + sitecode + "'" + ",'" + switchcode + "'" + ",'"+ trunckcode + "'" + ",'"+ mnpNum + "'";
    if(!dbInt)
	{
        logerror(m_strCRF,"BSRouting::DB_mroute_get_tdm_ip_address() DB Connection failure");
        return retCode::BS_DB_CONNECTION_FAILED;
    }
    CA::DBMAP dbMrouteGettTdmIpAddress;
    retCode ret = executeQuery(dbInt,spName,spValue,dbMrouteGettTdmIpAddress);
    logdebug(m_strCRF,"BSRouting::%s() DataBase : %s,Query %s",__func__,getDataBaseName(dbInt->m_connectionString).c_str(),dbInt->getLastQuery().c_str());
    if(ret != retCode::BS_SUCCESS){
	logdebug(m_strCRF,"%sInitiateRouting DB_mroute_get_tdm_ip_address returned empty", APP);
        setLastKnownError(dbInt->getLastKnownError());
        ClearDBData(dbMrouteGettTdmIpAddress);
        return ret;
    }

    if(dbMrouteGettTdmIpAddress.size() == 0){
        setLastKnownError("No SQL Record Found");
        return retCode::BS_DB_NO_RECORD;
    }
    getData("ip_address",0,dbMrouteGettTdmIpAddress,m_tdmipaddress.ipaddress);
    getData("interface",0,dbMrouteGettTdmIpAddress,m_tdmipaddress.interface);
    ClearDBData(dbMrouteGettTdmIpAddress);
    return ret;
}

retCode BSRouting::DB_croute_get_codec_details(CA::DBInterface* dbInt,std::string group){
    std::string spName =  "croute_get_codec_details";
    std::string spValue = " '" + group + "'";
    if(!dbInt)
	{
        logerror(m_strCRF,"BSRouting::DB_croute_get_codec_details() DB Connection failure");
        return retCode::BS_DB_CONNECTION_FAILED;
    }
    CA::DBMAP dbCRouteGetCodecDetails;
    retCode ret = executeQuery(dbInt,spName,spValue,dbCRouteGetCodecDetails);
    logdebug(m_strCRF,"BSRouting::%s() DataBase : %s,Query %s",__func__,getDataBaseName(dbInt->m_connectionString).c_str(),dbInt->getLastQuery().c_str());
    if(ret != retCode::BS_SUCCESS){
        logdebug(m_strCRF, "%sDB_croute_get_codec_details DB:%s, Query:%s - failed %s", APP, m_strRoutingProfile.c_str(), dbInt->getLastQuery().c_str(), getLastKnownError().c_str());
        setLastKnownError(dbInt->getLastKnownError());
        ClearDBData(dbCRouteGetCodecDetails);
        return ret;
    }

    if(dbCRouteGetCodecDetails.size() == 0){
        setLastKnownError("No SQL Record Found");
        return retCode::BS_DB_NO_RECORD;
    }
    getData("trunk_codec",0,dbCRouteGetCodecDetails,m_codecdetails.trunkcodec);
    ClearDBData(dbCRouteGetCodecDetails);
    return ret;
}

retCode BSRouting::DirectSwitching(CA::CAMap<CA::DBInterface*>& dblist,BSParam* bsp,int recPos)
{
    // in this function we are switching the mswitch
    retCode ret = retCode::BS_ERROR_UNKOWN;
    std::cout<<"DB_mroute_get_tdm_ip_address -- if case"<<std::endl;
    std::unique_ptr<BSSwitching> switch_ptr (new BSSwitching(bsp,m_strSwitchingProfile));
    ret = switch_ptr->InitiateSwitching(dblist,bsp);
    m_switchPtr = std::move(switch_ptr);
    //UnLoad(bsp);
    //postToCRR();
    return ret;
}
void BSRouting::setUserInfo(BSParam* bsp)
{

    char buff[1024] = {0};
    std::string outGoingInterface = "rtp9";
    std::string tariffClassA = "";
    std::string accessType = "";
    std::string custCode = "";
    memset(buff, 0, sizeof(buff));
    sprintf(buff, "%s:%s:%s:%s:%s:%s:%f:%s:1:A:1:%d:0:%s:%s:2:%s:%s::%f:%s:%d:%d:%f",
		m_tdmipaddress.interface.c_str(), m_strInterTrunkgroup.c_str(),m_strInterTrunkgroup.c_str(), m_strSiteCode.c_str(),
		m_strSiteCode.c_str(),m_strInterTrunkgroup.c_str(),bsp->getcallCost(), m_prefixRefData.prefixRef.c_str(), 
		0,bsp->getSiteCode().c_str(),tariffClassA.c_str(), accessType.c_str(), 
		m_prefixRefData.prefixRef.c_str(), bsp->getcallCost(),custCode.c_str(), 
		0, 0, bsp->getcallCost());
    bsp->setUinfo(buff);

}

void BSRouting::setPrivateInfo(BSParam* bsp)
{
    std::string temp;
    temp = "cdsbwdXs22jJZ4171303202315192800001:127.0.0.1:rtp9";
    bsp->setPinfo(temp);
}
retCode BSRouting::DirectSwitchingBypass(CA::DBInterface* dbInt,BSParam* bsp,int recPos)
{
    // In this function we are bypass the mswitch and get the xgate ip .
    //UnLoad(bsp);
    return retCode::BS_SUCCESS;
}

void BSRouting::UnLoad(BSParam* bsp,std::time_t connecttime,std::time_t disconnecttime,std::time_t starttime)
{
#if 0
  m_crrUpdate->setTalkTime(connecttime);
  time_t now = std::time(0);
  int talktime = 0;

  if(connecttime <= 0  || disconnecttime <= 0){
      talktime = 0;
   }else{
        talktime =  difftime(disconnecttime,connecttime);
         if(talktime <= 0)
            talktime = 0;

        }
  m_crrUpdate->setCallCause(talktime);
  m_crrUpdate->setHoldTime(talktime);
#endif 
  //clearing validRoute vector
  for (R25GetRoute* route : validRoute) {
    delete route;  
  }
  validRoute.clear(); 
  #if 0 
  if(m_switchPtr){
        m_switchPtr->UnLoad(bsp,connecttime,disconnecttime,starttime);
  }
  postToCRR();
 #endif 
}
