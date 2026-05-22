#ifndef CA_BUSINESS_CONFIG_HANDLER_H
#define CA_BUSINESS_CONFIG_HANDLER_H

#include "CAUtilityInternal.h"
#include "CAlog.h"

class DBProfile{
private:
std::string m_profileName;
std::string m_conString;
std::string m_conTimeout;
int m_conCount;
std::string m_dbtype;
public:
DBProfile()=default;
~DBProfile()=default;

void setProfileName(std::string str){ m_profileName = str;}
void setConnectionString(std::string str){ m_conString = str;}
void setConnectionTimeout(std::string timeout){m_conTimeout = timeout;}
void setConnectionCount(int count){m_conCount =  count;}
void setDBType(std::string dbtype){ 
   (dbtype.empty()) ?  m_dbtype="mssql" : m_dbtype = dbtype;
}

std::string getProfileName(){return m_profileName;}
std::string getConnectionString(){return m_conString;}
std::string getDBType(){return m_dbtype;}
std::string getConntetionTimeout(){return m_conTimeout;}
int getConnectionCount(){return m_conCount;}
};

class Application{
   private:
   std::string m_taskName;
   std::string m_espdb;
   std::string m_ccaas;
   std::string m_espparamdb;
   std::string m_mnpdb; //npopr
   std::string m_routingdb;
   std::string m_switchdb;
   std::string m_hlrdb;
   std::string m_appName;
   /*
   std::string m_hlrurl;
   std::string m_hlrussd;
   std::string m_outgoingTrunk;
   std::string m_iwmscPrefix;
   std::string m_mvnoPrefix;
   */

   public:
   Application()=default;
   ~Application()=default;
   
   void setTaskName(std::string str){m_taskName =  str;}
   void setEspDB(std::string str){m_espdb =  str;}
   void setCcaasDB(std::string str){m_ccaas =  str;}
   void setEspParamDB(std::string str){m_espparamdb =  str;}
   void setMnpDB(std::string str){m_mnpdb =  str;}
   void setRoutingDB(std::string str){m_routingdb =  str;}
   void setSwitchDB(std::string str){m_switchdb =  str;}
   void setHlrDB(std::string str){m_hlrdb = str;}
   void setAppName(std::string str){m_appName =  str;}
   /*
   void setHlrUrl(std::string str){m_hlrurl = str;}
   void setHlrUssd(std::string str){m_hlrussd = str;}
   void setOutgoingTrunk(std::string str){m_outgoingTrunk = str;}
   void setIwmscPrefix(std::string str){m_iwmscPrefix = str;}
   void setMvnoPrefix(std::string str){m_mvnoPrefix = str;}
   */

   std::string getTaskName(){return m_taskName;}
   std::string getEspDB(){return m_espdb;}
   std::string getCcaasDB(){return m_ccaas;}
   std::string getEspParamDB(){return m_espparamdb;}
   std::string getMnpDB(){return m_mnpdb;}
   std::string getSwitchDB(){return m_switchdb;}
   std::string getRoutingDB(){return m_routingdb;}
   std::string getHlrDB(){return m_hlrdb;}
   std::string getAppName(){return m_appName;}
   /*
   std::string getHlrUrl(){return m_hlrurl;}
   std::string getHlrUssd(){return m_hlrussd;}
   std::string getOutgoingTrunk(){return m_outgoingTrunk;}
   std::string getIwmscPrefix(){return m_iwmscPrefix;}
   std::string getMvnoPrefix(){return m_mvnoPrefix;}
   */
  void copy(Application app){
    setTaskName(app.getTaskName());
    setEspDB(app.getEspDB());
    setEspParamDB(app.getEspParamDB());
    setMnpDB(app.getMnpDB());
    setRoutingDB(app.getRoutingDB());
    setSwitchDB(app.getSwitchDB());
    setHlrDB(app.getHlrDB());
    setAppName(app.getAppName());
}

};
class Group{
   private:
   std::string m_cli;
   std::string m_ddi;
   std::string m_grpName;
   //CA::CAList<std::string> m_taskList;
   std::list<std::string> m_taskList;
   public:
   Group()=default;
   virtual ~Group(){
      m_taskList.clear();
   };
   void setCli(std::string str){m_cli = str;}
   void setDdi(std::string str){m_ddi = str;}
   void setGroupName(std::string str){m_grpName =  str;}
   void insertTaskList(std::string appStr){ m_taskList.push_back(appStr);}

   std::string getCli(){return m_cli;}
   std::string getDdi(){return m_ddi;}
   std::string getGroupName(){return m_grpName;}
   std::string popTaskList(){return (std::string)m_taskList.front(); m_taskList.pop_front();}
   std::list<std::string>* getSpawnTaskList(){ return &m_taskList;}

};


class Trunk{
   private:
   std::string m_trunkName;
   std::string m_ipSrc;
   std::string m_ipDest;
   int m_maxChannel;
   std::string m_siteCode;
   std::string m_cliDBProfile;
   std::string m_taskName;
   std::string m_iwmscPrefix;
   std::string m_mvnoPrefix;
   std::string m_hlrUrl;
  // CA::CAList<std::string> m_grpList;
  //std::list<std::string> m_grpList;
  std::list<std::string> m_ipList;
  
   public:
   Trunk()=default;
   virtual ~Trunk(){
      m_ipList.clear();
   };
   void setTrunkName(std::string str){m_trunkName =  str;}
   void setIpSrc(std::string str){m_ipSrc = str;}
   void setIpDest(std::string str){m_ipDest = str;}
   void setMaxChannel(int ch){m_maxChannel = ch;}
   void setSiteCode(std::string str){ m_siteCode = str;}
   void setCliDBProfile(std::string str) {m_cliDBProfile = str;}
   //void insertGroupList(std::string grpName){m_grpList.push_back(grpName);}
   void insertIpList(std::string ipAddr){m_ipList.push_back(ipAddr);}
   void setTaskName(std::string str){m_taskName =  str;}
   void setIwmscPrefix(std::string str){m_iwmscPrefix =  str;}
   void setMvnoPrefix(std::string str){m_mvnoPrefix =  str;}
   void setHlrUrl(std::string str){m_hlrUrl =  str;}

   std::string getTrunkName(){return m_trunkName;}
   std::string getIpSrc(){return m_ipSrc;}
   std::string getIpDest(){return m_ipDest;}
   std::string getSiteCode(){ return m_siteCode;}
   std::string getCliDBProfile() {return m_cliDBProfile;}
   std::string getTaskName(){return m_taskName;}
   std::string getIwmscPrefix(){return m_iwmscPrefix;}
   std::string getMvnoPrefix(){return m_mvnoPrefix;}
   std::string getHrlUrl(){return m_hlrUrl;}
   int getMaxChannel(){return m_maxChannel;}
   //std::string popGroupList(){return (std::string)m_grpList.front(); m_grpList.pop_front();}
   //std::list<std::string>* getGroupList(){ return &m_grpList;}
   

};

class prefix{
   private:
   std::string m_didPrefix;
   std::string m_trunkName;
   public:
   void setddiPrefix(std::string str){m_didPrefix = str;}
   void settrunkName(std::string str) {m_trunkName = str;}
   std::string getddiPrefix(){return m_didPrefix;}
   std::string gettrunkName() {return m_trunkName;}
};

 class MonitorConfig
{
   public:
      MonitorConfig()=default;
      ~MonitorConfig()=default;

      SET(unsigned short, i, Port);
      GET(i, Port);
      SET(unsigned short, i, ConnectionTimeout);
      GET(i, ConnectionTimeout);
      SET(unsigned short, i, ResponseTimeout);
      GET(i, ResponseTimeout);
      SET(STR, str, Ip);
      GET(str, Ip);

   private:
      unsigned short m_iPort;
      unsigned short m_iConnectionTimeout;
      unsigned short m_iResponseTimeout;
      STR m_strIp;
};

class BusinessConfig{
   private:
   std::string m_lastKnownError;
   CA::RapidXML rapidXML;
   rapidxml::xml_node<>*m_root;
   CA::CAMap<Trunk*> m_trunks;
   CA::CAMap<Group*> m_groups;
   CA::CAMap<Application*> m_apps;
   CA::CAMap<DBProfile*> m_dbprofile;
   std::list<prefix*> m_ddiPrefix;
   std::string m_listenPort;
   std::string m_logLevel;
   std::string m_logPath;
   std::string m_logFileSize;
   std::string m_logRetention;
   std::string m_switchCode;
   std::string m_cdrPath; 
   std::string m_connString;
   std::string m_userId; 
   std::string m_password;
   std::string m_localDBType;
   MonitorConfig m_tMonitorConfig;
   std::string m_callTimeout;
   std::string m_strDBWaitTimeOut;

   
   public:
   BusinessConfig()=default;
   virtual ~BusinessConfig();

   void setLastKnownError(std::string err);
   std::string getLastKnownError();
   std::string getlistenPort(){
      return m_listenPort;
   }
   std::string getLogLevel(){
      return m_logLevel;
   }
   std::string getlogPath(){
      return m_logPath;
   }
   std::string getLogFileSize(){
      return m_logFileSize;
   }
   std::string getLogRetention(){
      return m_logRetention;
   }
   std::string getSwitchCode(){
      return m_switchCode;
   }
   std::string getCdrPath(){
      return m_cdrPath;
   }
   std::string getLocalDBType(){
      return m_localDBType;
   }
   std::string getConnString(){
      return m_connString;
   }
   std::string getUserId(){
      return m_userId;
   }
   std::string getPassword(){
      return m_password;
   }
   MonitorConfig getMonitorConfig(){
      return m_tMonitorConfig;	   
   }
   std::string getCallTimeOut(){
      return m_callTimeout;
   }
   std::string getDBWaitTimeOut(){
      return m_strDBWaitTimeOut;
   }

   bool LoadRoot(std::string path , std::string fileName);
   bool LoadTrunk();
   bool LoadGroup();
   bool LoadApplication();
   bool LoadDBProfile();
   bool LoadLocalDBProfile();
   bool LoadBusinessServerSocket();
   bool LoadLogSetting();
   bool LoadGlobalSetting();
   bool LoadPrefix();
   bool LoadMonitorConfig();
   bool copyConfig(BusinessConfig& remConf);
   bool unLoadConfig();
   Trunk* getTrunkInfo(std::string url);
   //Group* getGroupInfo(std::string grpname);
   Application* getApplicationInfo(std::string taskName);
   DBProfile* getDBProfile(std::string profileName);
   CA::CAMap<DBProfile*>* getDBProfileList(){return &m_dbprofile;};
   CA::CAMap<Application*>*getApplicationList(){return &m_apps;};
   CA::CAMap<Trunk*>*getTrunkList(){return &m_trunks;};
   std::list<prefix*>* getPrefixList(){return &m_ddiPrefix;};
};

class BusinessConfigHandler

{
   private:
   BusinessConfig m_businessConfigObj;
   public:
      BusinessConfigHandler()=default;
      virtual ~BusinessConfigHandler()=default;
      bool LoadConfiguration();
      bool reLoadConfiguraton();
      std::unique_ptr<BusinessConfig> fetchConfigObject();
      std::string getServerPort(){
         return m_businessConfigObj.getlistenPort();
      }
      std::string getCallTimeOut(){
         return m_businessConfigObj.getCallTimeOut();
      }
      BusinessConfig* getConfigObj(){return &m_businessConfigObj;}
      bool getAppBasedDBProfile(std::string appName,CA::CAMap<DBProfile*>*);

      

      

      
};
#endif
