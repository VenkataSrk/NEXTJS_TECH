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
//#include "xGateHmpGstPool.h"
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

typedef ACE_Hash_Map_Manager<ACE_TString, xGateHmpGstPipeline*, ACE_SYNCH_RW_MUTEX> HASH_CHANNEL_MAP;

class xGateHmpGstManager {
  public:
    
    /**
     *  Default Constructor
     */
    xGateHmpGstManager (const char*);

    /**
     * Destructor
     */
    virtual ~xGateHmpGstManager (void);

    /**
     * Initialize the Hmp Gstreamer Manager class and thread
     */
    xGateRetVal init (void);

    /**
     * Start the event loop for processing messages/notifications. This method
     * must be static in order to start event loop processing in a newly spawned thread.
     * Also, this is an infinitely blocking call.
     */
    static void run (void* arguments);

    /**
     *  stopThread( void );
     */ 
    xGateRetVal stopThread( void );

    /**
     *  acquire_channel() on client request;
     */ 
    // xGateRetVal acquire_channel(MgMediaDetail &mediaDetail);
     xGateRetVal handle_dtmf_request(MgMediaDetail &mediaDetail);
     //Creating Orgination (leg-A) side port and IP.
     xGateRetVal allocate_channel_sockport(MgMediaDetail &mediaDetail);
     xGateRetVal handle_18X_response(MgMediaDetail &mediaDetail);   
     //Ring tone playback in (Leg-A) side rbt channel.
     xGateRetVal handle_rbt_playback(MgMediaDetail &mediaDetail);  
     xGateRetVal handle_rbt_stopback(MgMediaDetail &mediaDetail); 

     xGateRetVal handle_play_back(MgMediaDetail &mediaDetail);

     xGateRetVal allocate_term_channel(MgMediaDetail &mediaDetail);
     xGateRetVal allocate_orig_channel(MgMediaDetail &mediaDetail);
     xGateRetVal connect_media_channel(MgMediaDetail &mediaDetail);
 
    /**
     *  release_channel() called on client requst;
     */
     xGateRetVal release_channel(MgMediaDetail &mediaDetail);

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
   // xGateRetVal constructMediaElements();

    /** Boolean Flag
     * True  - The Thread is running
     * False - The Thread is stopped.
     */
    bool m_running;
    char ssrc[100];
    char cname[100];
    char mslabel[100];
    char label[100];
    char ice_ufrag[100];
    char remote_ufrag[100];
    char icepwd[100];
    char ice_candi[100];
    char fingerprt[250];
    string sslCertFileName;

    /// Char Server Type
    char m_serverType[SERVER_TYPE_SIZE];
#if 0
    ///pool objects
    HmpGstPool<xGateHmpGstBin> *m_pG711SendBinPool;
    HmpGstPool<xGateHmpGstBin> *m_pG711RecvBinPool;
    HmpGstPool<xGateHmpGstBin> *m_pG729SendBinPool;
    HmpGstPool<xGateHmpGstBin> *m_pG729RecvBinPool;
    HmpGstPool<xGateHmpGstPipeline> *m_pPipelinePool;
    HmpGstPool<xGateHmpGstUdpSrc> *m_pUdpSrcPool;
    HmpGstPool<xGateHmpGstUdpSink> *m_pUdpSinkPool;
#endif
    //Client* client;
    xGateHmpGstBin *m_pBin;
    xGateHmpGstPipeline *m_pPipeline;

    // allocated channel map
    HASH_CHANNEL_MAP m_acquiredChannelMap; //TODO: Yoga, reserve size of this map

    xGateHmpGstPipeline * create_pipeline(HmpGstPipelineType pipelineType);

    xGateRetVal create_udpbin_elements(xGateHmpGstPipeline *pHmpGstPipeline, ClientDetail &clientDetail);
    xGateRetVal remove_rtp_bins(xGateHmpGstPipeline *pHmpGstPipeline, Client *clientOrg);
    xGateRetVal set_client_details(ClientDetail &clientDetail, MgMediaDetail &mediaDetail);
    void setSecurityParams(Client *clientOrginator, ClientDetail &clientDetail);
    
    // acquiredChannelMap related functions
    xGateHmpGstPipeline * find_channel_entry_in_map(string callId); 
    xGateRetVal add_channel_entry_in_map(string callId, xGateHmpGstPipeline* hmpGstPipeline);
    xGateRetVal erase_channel_entry_in_map(string callId);

    void generateSSRC();
    void generateCNANE();
    void generateMSLABEL();
    void generateLABEL();
    void label_str(char* labelstr, int label_len);

    void generateUsername();
    void generatePassword();
    int  SetDTLSParams();
    bool generateSSLCertFile();
    bool MakeCertificateFile(EVP_PKEY*, X509*);
    void removePemFile(string &pemFile);
};

#endif
