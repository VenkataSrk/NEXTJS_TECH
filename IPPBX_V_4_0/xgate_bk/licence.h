/**
    $Header: /xGate/licence.h 14    1/25/05 4:52p Bennylp $
    $History: licence.h $
 * 
 * *****************  Version 14  *****************
 * User: Bennylp      Date: 1/25/05    Time: 4:52p
 * Updated in $/xGate
 * Reordering of LicenseManager startup in kernel.
 * 
 * *****************  Version 13  *****************
 * User: Bennylp      Date: 1/17/05    Time: 8:31p
 * Updated in $/xGate
 * Set default to HTTP URL and fix bug in concatenated URL.
 * 
 * *****************  Version 12  *****************
 * User: Bennylp      Date: 12/21/04   Time: 6:56p
 * Updated in $/xGate
 * Some modification to license
 * 
 * *****************  Version 11  *****************
 * User: Bennylp      Date: 12/07/04   Time: 2:27p
 * Updated in $/xGate
 * Merge support for HTTP license download
 * 
 * *****************  Version 10  *****************
 * User: Irwan        Date: 6/12/04    Time: 12:05
 * Updated in $/xGate
 * Web based license management request.
 * 
 * *****************  Version 9  *****************
 * User: Bennylp      Date: 12/03/04   Time: 5:57p
 * Updated in $/xGate
 * New license mechanism which includes Product ID, hostname, and IP
 * 
 * *****************  Version 8  *****************
 * User: Iwanj        Date: 30/04/02   Time: 12:24
 * Updated in $/xGate
 * add CompareFingerPrint(...)
 * 
 * *****************  Version 7  *****************
 * User: Iwanj        Date: 30/04/02   Time: 11:43
 * Updated in $/xGate
 * change implementation, RegisterSerialNum can be called anytime
 * verification is done everytime RegisterSerialNum is called
 * 
 * *****************  Version 6  *****************
 * User: Iwanj        Date: 29/04/02   Time: 16:20
 * Updated in $/xGate
 * move licence info to OnShowStatus
 * 
 * *****************  Version 5  *****************
 * User: Iwanj        Date: 29/04/02   Time: 13:55
 * Updated in $/xGate
 * move request generation to CommitConfiguration
 * 
 * *****************  Version 4  *****************
 * User: Iwanj        Date: 24/04/02   Time: 17:30
 * Updated in $/xGate
 * add  support  for disabled board using "XXX" for S/N
 * add additional info in licence file (product-ID and company name)
 * 
 * *****************  Version 3  *****************
 * User: Iwanj        Date: 23/04/02   Time: 14:07
 * Updated in $/xGate
 * modif Fg_XXX
 * 
 * *****************  Version 2  *****************
 * User: Iwanj        Date: 23/04/02   Time: 13:24
 * Updated in $/xGate
 * initial version
 * 
 * *****************  Version 1  *****************
 * User: Iwanj        Date: 15/04/02   Time: 14:41
 * Created in $/xGate
 * initial version
*/

#ifndef INC_LICENCE_MANAGER
#define INC_LICENCE_MANAGER

#include "serviceprovider.h"
//#include <windows.h>
//#include <wininet.h>

#define LICENCE_BUFSIZE	1024
#define MAX_SN_LEN	128
#define SN_UNKNOWN	"XXXXXXXXXXXXXXXX" \
			"XXXXXXXXXXXXXXXX" \
			"XXXXXXXXXXXXXXXX" \
			"XXXXXXXXXXXXXXXX" \
			"XXXXXXXXXXXXXXXX" \
			"XXXXXXXXXXXXXXXX" \
			"XXXXXXXXXXXXXXXX" \
			"XXXXXXXXXXXXXXXX"
#define HASH_LEN	32
#define NAME_LEN	64

enum LicenceMode
{
    Lm_Evaluation = 0,
    Lm_NodeLocked
};

enum FingerPrintId
{
    Fg_Signature = 0,
    Fg_Processor = 1,
    Fg_Disk,
    Fg_Ethernet,
    Fg_Aculab,
    Fg_Audiocodes,
    Fg_Dialogic,
    Fg_Atm,
    Fg_Mapletree,
    Fg_Hostname,
    Fg_IpAddress,
    Fg_Product,
    Fg_Prosody,
    Fg_PowerAccess,
    Fg_Unknown,
};

struct FingerPrint
{
    FingerPrint() {
	tag_ = Fg_Unknown;
	min_ = 0;
	len_ = 0;
	memset(val_,0,MAX_SN_LEN);
    }

    FingerPrint(FingerPrintId fg,int min,const unsigned char *data,int len) {
	tag_ = fg;
	min_ = min;
	memcpy(val_,data,len);
	len_ = len;
    }

    FingerPrintId tag_;
    unsigned char min_;
    unsigned char len_;
    unsigned char val_[MAX_SN_LEN];
};

class Cfg_Tree_Group;
class Cfg_Tree_Entry;
class Term_Cmd;

class LicenceManager: public ServiceProvider
{
public:
    LicenceManager();
    LicenceManager(const LicenceManager&);
    LicenceManager& operator=(const LicenceManager&);
    virtual ~LicenceManager();

    static LicenceManager* Create();
    static LicenceManager* Instance();

    virtual bool Load();
    virtual bool Unload();
    virtual bool CommitConfiguration();
    virtual bool KernelCommitConfiguration();
    virtual bool OnShowConfig(const Cfg_Tree_Entry*,String&,const String&,int) const;
    virtual bool OnShowStatus(String&);
    void DoTermCmd(Term_Session*,bool,Term_Cmd*);
    inline Cfg_Tree_Group* GetConfigTree();
    virtual void HandleEvents();

    //void RegisterSerialNum(FingerPrintId,const char*,int);
    void RegisterSerialNum(FingerPrintId,char,const char*,int);
    LicenceMode GetCurrentLicenceMode() const;

private:
    enum
    {
	Term_Cmd_LM_Config,
	Term_Cmd_LM_ShowRequest,
	Term_Cmd_LM_ExportRequest,
	Term_Cmd_LM_LicenceFile,
	Term_Cmd_LM_Reload,
    };

    //void GenerateLicenceRequest();
    int VerifyLicence();
    void SendErrorMessage(Term_Session* session,const char* msg);

    void ReloadLicense();

    //void RegisterPentium3SerialNum();
    void RegisterDisk0SerialNum();
    void RegisterMacAddress();
    void RegisterHostnameAndIpAddress();
    void TrimSpace(char*);
    //void ProtectBuffer();
    int ExportRequest(const char*);
    const char* CreateRequest();
    //int DecodeRequest(const char*,FingerPrint*&);
    //int CompareFingerPrint(FingerPrint*,int,FingerPrint*,int);

    int ReadLicenceFile();
    int CreateListFromLicence(const char*);
    int CompareFingerPrint();
    int CompareFingerPrint(FingerPrintId,char,const unsigned char*,int);

    //download license
    int URLtoAddressPort(char *url, char *addr, int *port, char *page);
    int SaveDownloadBuffer(int hin);
    int DownloadLicenceFile(Term_Session* session, const char *url);
    bool IsHttpLicence() const;

    static LicenceManager *instance_;
    Cfg_Tree_Group* cfgTree_;

    /*	these buffers will be allocated using VirtualAlloc so we can use 
	VirtualProtect to set access mode to this buffer (noaccess,readonly)
    */
    //unsigned char *fgprint_;
    //char *request_;

    char licence_file_[4096];
    char licence_url_[4096];
    char product_id_[HASH_LEN+1];
    char company_[NAME_LEN+1];
    //int fg_len_,total_fgprint_;
    LicenceMode mode_;
    long time0_;
    bool running_;
    bool http_licence_;

    typedef List<FingerPrint*> FingerPrintList;
    typedef List<FingerPrint*>::iterator FingerPrintListIter;
    FingerPrintList fgsys_,fgkey_;
};

#endif
