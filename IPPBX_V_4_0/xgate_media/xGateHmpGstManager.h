#ifndef _XGATE_HMP_GST_MANAGER_H
#define _XGATE_HMP_GST_MANAGER_H
/* 1. This thread object will manage gstreamer events, pipelines, bin and elements
*  2. full name of xGateHmpGstManager is Hmp(Host Media Processing), Gst(Gstreamer) Manager
*  3. It will manage gstreamer sources by using xGateHmpGstBin class
*/

//ace include
//#include <ace/Task.h>
#include <ace/Hash_Map_Manager.h>
#include <ace/Null_Mutex.h>

//local include
#include "xGateUtil.h"
#include "xGateHmpGstBin.h"
#include "stringencode.h"
#include <openssl/evp.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/rand.h>

#define ROOT_TMP_DIR "/tmp/dtlssrtp"
#define TMP_DIR_TEMPLATE ROOT_TMP_DIR "/XXXXXX"
#define FILE_PERMISIONS (S_IRWXU | S_IRWXG | S_IRWXO)
#define USERNAME_LENGTH 16
#define PASSWORD_LENGTH 24
#define SSRC_LENGTH 8
#define CNAME_LENGTH 16
#define MSLABEL_LENGTH 36
#define TEMPLATE_STRING "TEMPLATE_STRING"

//hash map for call entries & related functions
typedef ACE_Hash_Map_Manager<ACE_TString, xGateHmpCall*,  ACE_SYNCH_RW_MUTEX> HASH_HMP_CALL_MAP;

class xGateHmpGstManager {
  public:
    
    /**
     *  Default Constructor
     */
    xGateHmpGstManager(const char*);

    /**
     * Destructor
     */
    virtual ~xGateHmpGstManager(void);

    /**
     * Initialize the Hmp Gstreamer Manager class and thread
     */
    xGateRetVal init(void);

    /**
     * Start the event loop for processing messages/notifications. This method
     * must be static in order to start event loop processing in a newly spawned thread.
     * Also, this is an infinitely blocking call.
     */
    static void run(void* arguments);

    /**
     *  stopThread( void );
     */ 
    xGateRetVal stopThread( void );

    xGateRetVal allocate_receive_channel(MgMediaDetail &mediaDetail, xGateNetConnection &pbxConn);
    xGateRetVal connect_receive_channel(MgMediaDetail &mediaDetail);
    xGateRetVal release_receive_channel(MgMediaDetail &mediaDetail);
    xGateRetVal media_delete_channel(MgMediaDetail &mediaDetail);
    xGateRetVal handle_call_record(MgMediaDetail &mediaDetail);
 
  private:

    /**
     * Private copy constructor - disallow copies and automatic methods.
     **/
    xGateHmpGstManager(const xGateHmpGstManager& rhs);

    /**
     * Private assignment operator - disallow assignment and automatic methods.
     **/
    xGateHmpGstManager& operator= (const xGateHmpGstManager& rhs);

    /**
     * Init Gstreamer Library
     */
    xGateRetVal initGstreamerLib();

    /** 
     * Load required gstreamer elements, bin and pipelines
     */
    /** Boolean Flag
     * True  - The Thread is running
     * False - The Thread is stopped.
     */
    bool m_running;
    char m_serverType[SERVER_TYPE_SIZE];

    //hash map for call entries & related functions
    HASH_HMP_CALL_MAP m_hmpCallMap; //TODO: Yoga, reserve size of this map
    xGateHmpCall* find_call_entry(string callId); 
    xGateRetVal add_call_entry(string callId, xGateHmpCall *pHmpCall);
    xGateRetVal erase_call_entry(string callId);

    xGateRetVal set_media_details(xGateHmpCall *pHmpCall, MgMediaDetail &mediaDetail, ClientDetail &clientDetail);
    xGateRetVal update_media_details(ClientDetail &clientDetail, MgMediaDetail &mediaDetail);
};
#endif
