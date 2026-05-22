#ifndef _XGATE_BUSINESS_MANAGER_H
#define _XGATE_BUSINESS_MANAGER_H

//ace includes
#include <ace/Hash_Map_Manager.h>
#include <ace/Null_Mutex.h>

//local includes
#include "xGateUtil.h"
#include "xGateBusinessMsg.h"
#include "xGateBusinessInfo.h"

//hash map to store business info for particular call
typedef ACE_Hash_Map_Manager<ACE_TString, xGateBusinessInfo*,  ACE_SYNCH_RW_MUTEX> HASH_BUSINESS_INFO_MAP;

class xGateBusinessManager
{
  public:
    static xGateBusinessManager * getInstance();
    static void deleteInstance();

    //member functions
    bool process_msg(xGateBusinessMsg *pMsg); 

  private:
    xGateBusinessManager();
    ~xGateBusinessManager();

    //make noncopyable because of pointer
    xGateBusinessManager(const &);
    const xGateBusinessManager &operator=(const &); 

    //member functions
    bool init();
    bool deInit();
    bool parseJsonMsg(const char *jsonData, Document &doc);
    
    /* signalling layer message handling functions*/
    bool process_siglayer_msg(Document &doc);
    bool getInputDoc(Document &doc, const Value &inputVal);
    bool do_business_init(Document &doc, const Value &inputVal);
    bool get_aor(Document &doc, const Value &inputVal);
    bool update_CDR_Information(Document &doc,const Value &inputVal);

    /* db layer message handling functions */
    bool process_dblayer_msg(Document &doc);
    bool getOutputDoc(Document &doc, const Value &outputVal);
    bool read_prequery_response(Document &doc, const Value &outputVal); 

    /* scheduler message handling functions */
    bool process_scheduler_msg(Document &doc);

    /* hash map for business entries & related functions */
    HASH_BUSINESS_INFO_MAP m_businessInfoMap;
    xGateRetVal find_business_info_entry();
    xGateRetVal add_business_info_entry();
    xGateRetVal erase_business_info_entry();

    //member variable
    static xGateBusinessManager *m_mgrInstance;
    string m_uid;
    unsigned int m_opcode;  //this value might be either xGateBusinessEvent/xGateDbEvent
    xGateBusinessInfo *m_businessInfo;

};
#endif
