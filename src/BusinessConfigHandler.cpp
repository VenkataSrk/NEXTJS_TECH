#include "BusinessConfigHandler.h"
#include"LogHandler.h"

void BusinessConfig::setLastKnownError(std::string err){
    m_lastKnownError = err;
}
std::string BusinessConfig::getLastKnownError(){
    return m_lastKnownError;
}
bool BusinessConfig::LoadRoot(std::string path, std::string file){
    if(path.empty() || file.empty()){
        setLastKnownError("FilePath or FileName error");
       return  false;
    }

    CAFileHandle fileHandle;
    std::string fqfileName = path+file;
    std::vector<char>buf = fileHandle.readFile(fqfileName); 
    if(buf.empty()){
        setLastKnownError("file or data does not exit");
        return false;
    }
    rapidXML.setParserString(&buf[0]);
    rapidXML.parseDoc();
    m_root = rapidXML.getRootNode();
    if(!m_root){
        setLastKnownError("Exception in xml file root creation");
        return false;
    }
    setLastKnownError("Loaded Root Details" );
    return true;

}

bool BusinessConfig::LoadTrunk(){
    
    if(!m_root){
        setLastKnownError("xml doc not Loaded - LoadTrunk()");
        return false;
    }
    rapidxml::xml_node<>* sipTrunkNode = rapidXML.getSubNode("siptrunk", m_root);
    if(!sipTrunkNode){
        setLastKnownError("unable load sip trunk details");
        return false;
    }
    for(rapidxml::xml_node<>* trunkNode=rapidXML.getSubNode("trunk", sipTrunkNode);
    trunkNode;trunkNode=rapidXML.getNextSibling("trunk", trunkNode)){
        if(trunkNode){
            Trunk* trunkDetail = new Trunk();
            if(trunkDetail){
               rapidxml::xml_node<>* trunkNameNode = rapidXML.getSubNode("trunk_name",trunkNode);
               if(trunkNameNode){
                std::string value="";
                rapidXML.getNodeValue(trunkNameNode, value);
                trunkDetail->setTrunkName(value);
               } 
               rapidxml::xml_node<>* cliDBProfile =  rapidXML.getSubNode("cli_dbprofile",trunkNode);
               if(cliDBProfile){
                std::string value="";
                rapidXML.getNodeValue(cliDBProfile,value);
                trunkDetail->setCliDBProfile(value);
               }
               for(rapidxml::xml_node<>* ipSrcNode = rapidXML.getSubNode("ip_src",trunkNode); ipSrcNode; ipSrcNode = rapidXML.getNextSibling("ip_src",ipSrcNode)){
                if(ipSrcNode){
                    std::string value="";
                    rapidXML.getNodeValue(ipSrcNode,value);
                    //trunkDetail->setIpSrc(value);
                    trunkDetail->insertIpList(value);
                }
               }
                                                                          
                rapidxml::xml_node<>* taskNameNode = rapidXML.getSubNode("spawntask",trunkNode);
                if(taskNameNode){
                    std::string value = "";
                    rapidXML.getNodeValue(taskNameNode,value);
                    trunkDetail->setTaskName(value);
                }
                rapidxml::xml_node<>* maxChannelNode = rapidXML.getSubNode("maxchannel",trunkNode);
                if(maxChannelNode){
                    std::string value="";
                    rapidXML.getNodeValue(maxChannelNode,value);
                    trunkDetail->setMaxChannel(atoi(value.c_str()));
                }
                rapidxml::xml_node<>* siteCodeNode = rapidXML.getSubNode("site_code",trunkNode);
                if(siteCodeNode){
                    std::string value="";
                    rapidXML.getNodeValue(siteCodeNode,value);
                    trunkDetail->setSiteCode(value);
                }
                rapidxml::xml_node<>* iwmscPrefixNode = rapidXML.getSubNode("iwmsc_prefix",trunkNode);
                if(iwmscPrefixNode){
                    std::string value="";
                    rapidXML.getNodeValue(iwmscPrefixNode,value);
                    trunkDetail->setIwmscPrefix(value);
                }
                rapidxml::xml_node<>* mvnoPrefixNode = rapidXML.getSubNode("mvno_prefix",trunkNode);
                if(mvnoPrefixNode){
                    std::string value="";
                    rapidXML.getNodeValue(mvnoPrefixNode,value);
                    trunkDetail->setMvnoPrefix(value);
                }

                rapidxml::xml_node<>* hlrUrlNode = rapidXML.getSubNode("hlr_url",trunkNode);
                if(hlrUrlNode){
                    std::string value="";
                    rapidXML.getNodeValue(hlrUrlNode,value);
                    trunkDetail->setHlrUrl(value);
                }
#if 0 // this group is removed for latest configuration setup Yadav
                for(rapidxml::xml_node<>* groupNode=rapidXML.getSubNode("group",trunkNode);
                groupNode; groupNode = rapidXML.getNextSibling("group", groupNode)){
                    if(groupNode){
                        std::string value="";
                        rapidXML.getNodeValue(groupNode,value);
                        trunkDetail->insertGroupList(value);
                    }
                }
#endif
                // insert the details to Map Trunk
                m_trunks.InsertToMap(trunkDetail->getTrunkName(),trunkDetail);
            }
        }
    }
   setLastKnownError("Loaded Trunk Details" );
return true;
}

bool BusinessConfig::LoadGroup(){
    if(!m_root){
        setLastKnownError("xml doc not Loaded - LoadGroup()");
        return false;
    }
    rapidxml::xml_node<>* groupsNode = rapidXML.getSubNode("groups",m_root);
    if(!groupsNode){
        setLastKnownError("unable to load groups details");
        return false;
    }
    for(rapidxml::xml_node<>* grpNode=rapidXML.getSubNode("grp",groupsNode);
        grpNode;grpNode=rapidXML.getNextSibling("grp", grpNode)){
            if(grpNode){
                Group* grpDetails =  new Group();
                if(grpDetails){
                    rapidxml::xml_node<>* grpNameNode = rapidXML.getSubNode("grp_name",grpNode);
                    if(grpNameNode){
                        std::string value="";
                        rapidXML.getNodeValue(grpNameNode,value);
                        grpDetails->setGroupName(value);
                    }
                    rapidxml::xml_node<>* cliNode =   rapidXML.getSubNode("cli",grpNode);
                    if(cliNode){
                        std::string value="";
                        rapidXML.getNodeValue(cliNode,value);
                        grpDetails->setCli(value);

                    }
                    rapidxml::xml_node<>* ddiNode =   rapidXML.getSubNode("ddi",grpNode);
                    if(ddiNode){
                        std::string value="";
                        rapidXML.getNodeValue(ddiNode,value);
                        grpDetails->setDdi(value);
                    }
                    for(rapidxml::xml_node<>* taskNode = rapidXML.getSubNode("spawntask",grpNode);
                    taskNode; taskNode = rapidXML.getNextSibling("spawntask",taskNode)){
                        if(taskNode){
                            std::string value;
                            rapidXML.getNodeValue(taskNode,value);
                            grpDetails->insertTaskList(value);
                        }
                    }
                    m_groups.InsertToMap(grpDetails->getGroupName(),grpDetails);
                }
            }
        }
        setLastKnownError("Loaded Group Details");
        return true;
}

bool BusinessConfig::LoadApplication(){
    if(!m_root){
        setLastKnownError("xml doc not Loaded - LoadApplication()");
        return false;
    }
    for(rapidxml::xml_node<>* applicationNode =  rapidXML.getSubNode("Application",m_root);
        applicationNode; applicationNode = rapidXML.getNextSibling("Application",applicationNode)){
    
            Application* appDetails =  new Application();
            if(appDetails){
                std::string value = "";
                if (rapidXML.getFirstAttributeValue("task_name",applicationNode,value) == 1)                
                appDetails->setTaskName(value);

                value.clear();
                if(rapidXML.getFirstAttributeValue("app",applicationNode,value) ==  1)
                appDetails->setAppName(value);

                value.clear();
                if(rapidXML.getFirstAttributeValue("espdb",applicationNode,value) == 1)
                appDetails->setEspDB(value);

                value.clear();
                if(rapidXML.getFirstAttributeValue("espparamdb",applicationNode,value) ==1)
                appDetails->setEspParamDB(value);
                
                value.clear();
                rapidXML.getFirstAttributeValue("hlrdb",applicationNode,value);
                appDetails->setHlrDB(value);
                
                value.clear();
                rapidXML.getFirstAttributeValue("npopr",applicationNode,value);
                appDetails->setMnpDB(value);
                
                value.clear();
                rapidXML.getFirstAttributeValue("routing",applicationNode,value);
                appDetails->setRoutingDB(value);

                value.clear();
                if(rapidXML.getFirstAttributeValue("switch",applicationNode,value) ==1)
                appDetails->setSwitchDB(value);

                value.clear();
                if(rapidXML.getFirstAttributeValue("ccaasdb",applicationNode,value) ==1)
                appDetails->setCcaasDB(value);

                m_apps.InsertToMap(appDetails->getTaskName(),appDetails);

            }
        }
    setLastKnownError("Loaded Application Details");
    return true;
}

bool BusinessConfig::LoadDBProfile(){
    if(!m_root){
        setLastKnownError("xml doc not Loaded - LoadDBProfile()");
        return false;
    }
    for(rapidxml::xml_node<>* dbNode =  rapidXML.getSubNode("db_profile",m_root);
        dbNode; dbNode = rapidXML.getNextSibling("db_profile",dbNode)){
    
            DBProfile* dbDetails =  new DBProfile();
            if(dbDetails){
                std::string value = "";
                if (rapidXML.getFirstAttributeValue("profile_name",dbNode,value) == 1)                
                dbDetails->setProfileName(value);

                value.clear();
                if(rapidXML.getFirstAttributeValue("connection_string",dbNode,value) ==  1)
                dbDetails->setConnectionString(value);

                value.clear();
                if(rapidXML.getFirstAttributeValue("connection_timeout",dbNode,value) == 1)
                    if(value.empty() == false)
                    dbDetails->setConnectionTimeout(value);
               // dbDetails->setConnectionTimeout(atoi(value.c_str()));

                value.clear();
                if(rapidXML.getFirstAttributeValue("connection_count",dbNode,value) ==1)
                if(value.empty() == false)
                dbDetails->setConnectionCount(atoi(value.c_str()));

                value.clear();
                if(rapidXML.getFirstAttributeValue("dbtype",dbNode,value) == 1)
                if(value.empty() == false)
                dbDetails->setDBType(value);
                
                

                m_dbprofile.InsertToMap(dbDetails->getProfileName(),dbDetails);
                

            }
        }
setLastKnownError("Loaded DBProfile Details");
return true;
}

bool BusinessConfig::LoadLocalDBProfile(){
    if(!m_root){
        setLastKnownError("xml doc not Loaded - Server Socket Details()");
        return false;
    }
    rapidxml::xml_node<>* dbNode = rapidXML.getSubNode("local-db",m_root);
    if(!dbNode){
        setLastKnownError("Business Node failed in LoadBusinessServerSocket");
        return false;
    }
    rapidxml::xml_node<>* typeNode = rapidXML.getSubNode("db_type",dbNode);
    if(typeNode){
        m_localDBType.clear();
        rapidXML.getNodeValue(typeNode,m_localDBType);
    }
    rapidxml::xml_node<>* connNode = rapidXML.getSubNode("conn_string",dbNode);
    if(connNode){
        m_connString.clear();
        rapidXML.getNodeValue(connNode,m_connString);
    }
    rapidxml::xml_node<>* userNode = rapidXML.getSubNode("user",dbNode);
    if(userNode){
        m_userId.clear();
        rapidXML.getNodeValue(userNode,m_userId);
    }
    rapidxml::xml_node<>* passwdNode = rapidXML.getSubNode("passwd",dbNode);
    if(passwdNode){
        m_password.clear();
        rapidXML.getNodeValue(passwdNode,m_password);
    }
    setLastKnownError("Loaded ServerSocket Details");
    return true;
}

bool BusinessConfig::LoadBusinessServerSocket(){
    if(!m_root){
        setLastKnownError("xml doc not Loaded - Server Socket Details()");
        return false;
    }
    rapidxml::xml_node<>* SocNode = rapidXML.getSubNode("business",m_root);
    if(!SocNode){
        setLastKnownError("Business Node failed in LoadBusinessServerSocket");
        return false;
    }
    rapidxml::xml_node<>* portNode = rapidXML.getSubNode("port",SocNode);
    if(portNode){
        m_listenPort.clear();
        rapidXML.getNodeValue(portNode,m_listenPort);
        
        
    }
setLastKnownError("Loaded ServerSocket Details");
return true;
}
bool BusinessConfig::LoadGlobalSetting(){
    if(!m_root){
        setLastKnownError("xml doc not Loaded - Global Details()");
        return false;
    }
    rapidxml::xml_node<>* gloNode = rapidXML.getSubNode("global",m_root);
    if(!gloNode){
        setLastKnownError("Global Node failed in LoadGlobalSetting");
        return false;
    }
    rapidxml::xml_node<>* switchNode = rapidXML.getSubNode("SWITCH_CODE",gloNode);
    if(switchNode){
        m_switchCode.clear();
        rapidXML.getNodeValue(switchNode,m_switchCode);   
    }
    rapidxml::xml_node<>* cdrNode = rapidXML.getSubNode("CDR_PATH",gloNode);
    if(cdrNode){
        m_cdrPath.clear();
        rapidXML.getNodeValue(cdrNode,m_cdrPath);   
    }
    rapidxml::xml_node<>* callTimeoutNode = rapidXML.getSubNode("CALL_TIMEOUT",gloNode);       
    if(callTimeoutNode) {
        m_callTimeout.clear();
        rapidXML.getNodeValue(callTimeoutNode,m_callTimeout);
    }
    rapidxml::xml_node<>* dbWaitTimeNode = rapidXML.getSubNode("DB_WAITTIMEOUT",gloNode);
    if(dbWaitTimeNode) {
       m_strDBWaitTimeOut.clear();
       rapidXML.getNodeValue(dbWaitTimeNode,m_strDBWaitTimeOut);
    }
setLastKnownError("Loaded Global Details");
return true;

}
bool BusinessConfig::LoadLogSetting(){
    if(!m_root){
        setLastKnownError("xml doc not Loaded -Logging Details()");
        return false;
    }
    rapidxml::xml_node<>* logNode =  rapidXML.getSubNode("logging",m_root);
    if(!logNode){
        setLastKnownError("logging setting not loaded");
        return false;
    }
    rapidxml::xml_node<>* levelNode = rapidXML.getSubNode("level",logNode);
    if(levelNode){
       rapidXML.getNodeValue(levelNode,m_logLevel) ;
    }
    rapidxml::xml_node<>* pathNode = rapidXML.getSubNode("file_path",logNode);
    if(pathNode){
       rapidXML.getNodeValue(pathNode,m_logPath);
    }
    rapidxml::xml_node<>* sizeNode = rapidXML.getSubNode("max_file_size",logNode);
    if(sizeNode){
       rapidXML.getNodeValue(sizeNode,m_logFileSize);
    }

    rapidxml::xml_node<>* retenNode = rapidXML.getSubNode("retention",logNode);
    if(retenNode){
       rapidXML.getNodeValue(retenNode,m_logRetention);
    }

    setLastKnownError("Loaded Logging Details");
    return true;

}
bool BusinessConfig::LoadPrefix(){
    if(!m_root){
        setLastKnownError("xml doc not Loaded -prefix Details()");
        return false;
    }

   for( rapidxml::xml_node<>* prfNode =  rapidXML.getSubNode("prefix",m_root); prfNode; prfNode = rapidXML.getNextSibling("prefix",prfNode)){
    prefix *ddiprf =  new prefix();
    if(ddiprf){
        std::string value = "";
        if (rapidXML.getFirstAttributeValue("didprefix",prfNode,value) == 1){
            ddiprf->setddiPrefix(value);
        }
        value.clear();
        if (rapidXML.getFirstAttributeValue("trunkname",prfNode,value) == 1){
            ddiprf->settrunkName(value);
        }
        m_ddiPrefix.push_back (ddiprf);
    }
   }
   setLastKnownError("DDI Prefix Load completed");
    return true;
}

  bool BusinessConfig::LoadMonitorConfig()
{
    if(!m_root)
    {
        setLastKnownError("xml doc not Loaded -monitor Details()");
        return false;
    }
    rapidxml::xml_node<>* monNode =  rapidXML.getSubNode("monitor",m_root);
    if(!monNode)
    {
        setLastKnownError("monitor setting not loaded");
        return false;
    }
    STR val{""};
    rapidxml::xml_node<>* subNode = rapidXML.getSubNode("port",monNode);
    if(subNode)
    {
       rapidXML.getNodeValue(subNode,val);
       m_tMonitorConfig.setPort(atoi(val.c_str()));
    }
    subNode = rapidXML.getSubNode("connection_timeout",monNode);
    if(subNode)
    {
       rapidXML.getNodeValue(subNode,val);
       m_tMonitorConfig.setConnectionTimeout(atoi(val.c_str()));
    }
    subNode = rapidXML.getSubNode("response_timeout",monNode);
    if(subNode)
    {
       rapidXML.getNodeValue(subNode,val);
       m_tMonitorConfig.setResponseTimeout(atoi(val.c_str()));
    }
    subNode = rapidXML.getSubNode("ip",monNode);
    if(subNode)
    {
       rapidXML.getNodeValue(subNode,val);
       m_tMonitorConfig.setIp(val);
    }
    return true;
}

Trunk* BusinessConfig::getTrunkInfo(std::string url){
    if (url.empty()){
        setLastKnownError("url is emptry");
        return nullptr;
    }

    Trunk* trunkInfo = m_trunks.FindValue(url);
    
   return ( (trunkInfo) ?  trunkInfo :  nullptr);
}
#if 0 // group feature is diabled new config changes Yadav
Group* BusinessConfig::getGroupInfo(std::string grpname){
    if(grpname.empty() ){
        setLastKnownError("group Name is empty");
        return nullptr;
    }

    Group* grp  = m_groups.FindValue(grpname);

    return (grp) ? grp : nullptr;


}
#endif
Application* BusinessConfig::getApplicationInfo(std::string taskName){
    if(taskName.empty()){
        setLastKnownError("Task Name is empty");
        return nullptr;
    }
    Application* apps = m_apps.FindValue(taskName);

    return (apps) ? apps : nullptr;
}
DBProfile* BusinessConfig::getDBProfile(std::string profileName){
    if(profileName.empty()){
        setLastKnownError("DB Profile is empty");
        return nullptr;
    }
    DBProfile* dbpro = m_dbprofile.FindValue(profileName);

    return (dbpro) ? dbpro : nullptr;
}


BusinessConfig::~BusinessConfig(){
    unLoadConfig();
}

bool BusinessConfig::unLoadConfig(){
     //if(m_root){delete m_root; m_root = nullptr;}
     rapidXML.clearDoc();
#if 0
   for(CA::CAMap<Trunk*>::ca_itr itr =  m_trunks.Begin() ; itr != m_trunks.End(); itr++ ){
    Trunk* trunk =  itr->second;
    if(trunk){delete trunk; trunk = nullptr;}
   }
   #endif
    //m_trunks.Clear();
#if 0
   for(CA::CAMap<Group*>::ca_itr itr =  m_groups.Begin() ; itr != m_groups.End(); itr++ ){
    Group* grp = itr->second;
    if(grp){delete grp; grp = nullptr;}
   }
#endif
    //m_groups.Clear();
#if 0
   for(CA::CAMap<Application*>::ca_itr itr =  m_apps.Begin() ; itr != m_apps.End(); itr++ ){
     Application* app = itr->second;
     if(app){delete app; app = nullptr;}
   }
#endif
   //m_apps.Clear();
#if 0
   for(CA::CAMap<DBProfile*>::ca_itr itr =  m_dbprofile.Begin() ; itr != m_dbprofile.End(); itr++ ){
    DBProfile* dbPrf =  itr->second;
    if(dbPrf){delete dbPrf; dbPrf=nullptr;}
   }
#endif
   //m_dbprofile.Clear();
#if 0
   for(std::list<prefix*>::iterator itr = m_ddiPrefix.begin() ;  itr != m_ddiPrefix.end(); itr++){
    prefix* prf =  *itr;
    if(prf){delete prf; prf =  nullptr; }
   }
#endif
   //m_ddiPrefix.clear();

   return true;
}




bool BusinessConfigHandler::LoadConfiguration(){

    if(reLoadConfiguraton() == true)
        logstart(m_businessConfigObj.getLogLevel(),m_businessConfigObj.getlogPath(),m_businessConfigObj.getLogFileSize(),m_businessConfigObj.getLogRetention());
    return true;
}
bool BusinessConfigHandler::reLoadConfiguraton(){
    std::string configPath;
    std::string file = "conf.xml";
    const char* envConfPath = std::getenv("CONF_PATH");
    if (envConfPath && *envConfPath)
    {
	    configPath = envConfPath;
    }

    if(m_businessConfigObj.LoadRoot(configPath,file) == true){
        m_businessConfigObj.LoadBusinessServerSocket();
        m_businessConfigObj.LoadGlobalSetting();
        m_businessConfigObj.LoadApplication();
        m_businessConfigObj.LoadDBProfile();
        m_businessConfigObj.LoadLocalDBProfile();
        m_businessConfigObj.LoadMonitorConfig();
        m_businessConfigObj.LoadLogSetting();
        return true;
    }
    
return false;
}

std::unique_ptr<BusinessConfig> BusinessConfigHandler::fetchConfigObject(){
    std::string configPath;
    std::string file = "conf.xml";
    const char* envConfPath = std::getenv("CONF_PATH");
    if (envConfPath && *envConfPath)
    {
	    configPath = envConfPath;
    }

    std::unique_ptr<BusinessConfig> pConfig (new BusinessConfig);

    if(pConfig){
        if(pConfig->LoadRoot(configPath,file) == true){
            pConfig->LoadTrunk();
            pConfig->LoadGlobalSetting();
            pConfig->LoadApplication();
            pConfig->LoadDBProfile();
            pConfig->LoadPrefix();
            pConfig->LoadMonitorConfig();
            pConfig->LoadLogSetting();
        }
        return std::move(pConfig);
    }
    return nullptr;
}
 bool BusinessConfigHandler::getAppBasedDBProfile(std::string appName, CA::CAMap<DBProfile*>* appDBList){
    if(appName.empty())
        return false;

    
    if(!appDBList)
        return false;

    CA::CAMap<Application*>* appList = m_businessConfigObj.getApplicationList();
    if(!appList)
        return false;

    for(CA::CAMap<Application*>::ca_itr itr =  appList->Begin() ; itr != appList->End(); itr++ ){
        Application* app = itr->second;
        if(app){
            if(appName.compare(app->getAppName()) == 0){
                if(appName.compare("ESP") == 0){
                DBProfile* dbesp = m_businessConfigObj.getDBProfile(app->getEspDB());
                if(dbesp)
                    appDBList->InsertToMap(app->getEspDB(),dbesp);
                DBProfile* dbesppram = m_businessConfigObj.getDBProfile(app->getEspParamDB());
                if(dbesppram)
                    appDBList->InsertToMap(app->getEspParamDB(),dbesppram);
                DBProfile* dbmnp = m_businessConfigObj.getDBProfile(app->getMnpDB());
                if(dbmnp)
                    appDBList->InsertToMap(app->getMnpDB(),dbmnp);
                DBProfile* dbrout = m_businessConfigObj.getDBProfile(app->getRoutingDB());
                if(dbrout)
                    appDBList->InsertToMap(app->getRoutingDB(),dbrout);
                DBProfile* dbswitch = m_businessConfigObj.getDBProfile(app->getSwitchDB());
                if(dbswitch)
                    appDBList->InsertToMap(app->getSwitchDB(),dbswitch);
                DBProfile* dbccaas = m_businessConfigObj.getDBProfile(app->getCcaasDB());
                if(dbccaas)
                    appDBList->InsertToMap(app->getCcaasDB(),dbccaas);

                }else if(appName.compare("GMSC") == 0){
                    DBProfile* dbhlr = m_businessConfigObj.getDBProfile(app->getHlrDB());
                    if(dbhlr)
                        appDBList->InsertToMap(app->getHlrDB(),dbhlr);
                    DBProfile* dbmnp = m_businessConfigObj.getDBProfile(app->getMnpDB());
                    if(dbmnp)
                        appDBList->InsertToMap(app->getMnpDB(),dbmnp);
                    DBProfile* dbrout = m_businessConfigObj.getDBProfile(app->getRoutingDB());
                    if(dbrout)
                        appDBList->InsertToMap(app->getRoutingDB(),dbrout);
                    DBProfile* dbswitch = m_businessConfigObj.getDBProfile(app->getSwitchDB());
                    if(dbswitch)
                        appDBList->InsertToMap(app->getSwitchDB(),dbswitch);

                }
            }else if(appName.compare("ROUTSWITCH") == 0){
                DBProfile* dbrout = m_businessConfigObj.getDBProfile(app->getRoutingDB());
                if(dbrout)
                    appDBList->InsertToMap(app->getRoutingDB(),dbrout);
                DBProfile* dbswitch = m_businessConfigObj.getDBProfile(app->getSwitchDB());
                if(dbswitch)
                    appDBList->InsertToMap(app->getSwitchDB(),dbswitch);
            }
        }
    }
    if(appDBList->GetMapSize() == 0)
        return false;
    
    return true;
}

bool BusinessConfig::copyConfig(BusinessConfig& rconf){
    m_logLevel =  rconf.getLogLevel();
    m_logPath = rconf.getlogPath();
    m_logFileSize =  rconf.getLogFileSize(); 
    m_logRetention =  rconf.getLogRetention();
    m_switchCode =  rconf.getSwitchCode();
    m_callTimeout = rconf.getCallTimeOut();
    m_strDBWaitTimeOut = rconf.getDBWaitTimeOut();
    m_dbprofile.Copy(*rconf.getDBProfileList());
    m_trunks.Copy(*rconf.getTrunkList());
    m_apps.Copy(*rconf.getApplicationList());
    m_ddiPrefix.assign(rconf.getPrefixList()->begin(), rconf.getPrefixList()->end());
    return true;
}


