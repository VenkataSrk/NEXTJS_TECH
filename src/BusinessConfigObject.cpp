#include "BusinessConfigObject.h"

BSCallConfObject::BSCallConfObject(const BSCallConfObject& bsCCObj){

    m_trunkName = bsCCObj.m_trunkName;
    m_maxChannel =bsCCObj.m_maxChannel;
    m_groupName=  bsCCObj.m_groupName;
    m_cli=  bsCCObj.m_cli;
    m_ddi= bsCCObj.m_ddi;
    m_url = bsCCObj.m_url;
    m_uid = bsCCObj.m_uid;
    m_errorCode = bsCCObj.m_errorCode;
    m_siteCode = bsCCObj.m_siteCode;
    m_switchCode =  bsCCObj.m_switchCode;
    
    
}



void BSDBProfileObject::copyDBProfilList(CA::CAMap<DBProfile*> *profileList){

    m_dbProObject.Copy(*profileList);
 
}

void BSDBProfileObject::getDBProfilDetails(){
    for (CA::CAMap<DBProfile*>::ca_itr it = m_dbProObject.Begin(); it != m_dbProObject.End(); it++){
        DBProfile* db = it->second;
        std::cout << "COUNT: " << db->getConnectionCount() << std::endl;
        std::cout << "Connection String: " << db->getConnectionString() << std::endl;
        std::cout << "Time Out: " << db->getConntetionTimeout() << std:: endl;
        std::cout << "Profile Name: "  << db->getProfileName() << std::endl;
        
    }
}


