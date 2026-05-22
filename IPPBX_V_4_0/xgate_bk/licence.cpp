/**
    $Header: /xGate/licence.cpp 25    1/25/05 4:52p Bennylp $
    $History: licence.cpp $
 * 
 * *****************  Version 25  *****************
 * User: Bennylp      Date: 1/25/05    Time: 4:52p
 * Updated in $/xGate
 * Reordering of LicenseManager startup in kernel.
 * 
 * *****************  Version 24  *****************
 * User: Bennylp      Date: 1/24/05    Time: 11:40a
 * Updated in $/xGate
 * Fixed bug, URL in config gets overwritten everytime.
 * 
 * *****************  Version 23  *****************
 * User: Bennylp      Date: 1/20/05    Time: 7:26p
 * Updated in $/xGate
 * Added timeout.
 * 
 * *****************  Version 22  *****************
 * User: Bennylp      Date: 1/20/05    Time: 5:56p
 * Updated in $/xGate
 * Version 60.3 (removed WinINET from LM)
 * 
 * *****************  Version 21  *****************
 * User: Bennylp      Date: 1/18/05    Time: 6:31p
 * Updated in $/xGate
 * Send xGate version in browser info.
 * 
 * *****************  Version 20  *****************
 * User: Bennylp      Date: 1/17/05    Time: 8:31p
 * Updated in $/xGate
 * Set default to HTTP URL and fix bug in concatenated URL.
 * 
 * *****************  Version 19  *****************
 * User: Bennylp      Date: 12/21/04   Time: 5:02p
 * Updated in $/xGate
 * Added more commands.
 * 
 * *****************  Version 18  *****************
 * User: Bennylp      Date: 12/09/04   Time: 3:34p
 * Updated in $/xGate
 * Don't show licence command if not configured
 * 
 * *****************  Version 17  *****************
 * User: Bennylp      Date: 12/07/04   Time: 2:27p
 * Updated in $/xGate
 * Merge support for HTTP license download
 * 
 * *****************  Version 16  *****************
 * User: Irwan        Date: 6/12/04    Time: 12:05
 * Updated in $/xGate
 * Web based license management request.
 * 
 * *****************  Version 15  *****************
 * User: Bennylp      Date: 12/03/04   Time: 5:57p
 * Updated in $/xGate
 * New license mechanism which includes Product ID, hostname, and IP
 * 
 * *****************  Version 14  *****************
 * User: Iwanj        Date: 8/05/02    Time: 20:24
 * Updated in $/xGate
 * remove disk registration
 * 
 * *****************  Version 13  *****************
 * User: Iwanj        Date: 8/05/02    Time: 17:41
 * Updated in $/xGate
 * 
 * *****************  Version 12  *****************
 * User: Iwanj        Date: 8/05/02    Time: 17:37
 * 
 * *****************  Version 11  *****************
 * User: Iwanj        Date: 30/04/02   Time: 12:24
 * Updated in $/xGate
 * add CompareFingerPrint(...)
 * 
 * *****************  Version 10  *****************
 * User: Iwanj        Date: 30/04/02   Time: 11:43
 * Updated in $/xGate
 * change implementation, RegisterSerialNum can be called anytime
 * verification is done everytime RegisterSerialNum is called
 * 
 * *****************  Version 9  *****************
 * User: Iwanj        Date: 29/04/02   Time: 16:42
 * Updated in $/xGate
 * fixing due to DoTermCmd and CommitConfig firing sequence
 * 
 * *****************  Version 8  *****************
 * User: Iwanj        Date: 29/04/02   Time: 16:22
 * Updated in $/xGate
 * adjust output alignment
 * 
 * *****************  Version 7  *****************
 * User: Iwanj        Date: 29/04/02   Time: 16:20
 * Updated in $/xGate
 * move licence info to OnShowStatus
 * 
 * *****************  Version 6  *****************
 * User: Iwanj        Date: 29/04/02   Time: 13:55
 * Updated in $/xGate
 * move request generation to CommitConfiguration
 * 
 * *****************  Version 5  *****************
 * User: Iwanj        Date: 24/04/02   Time: 18:57
 * Updated in $/xGate
 * reset info on unmatched S/N
 * 
 * *****************  Version 4  *****************
 * User: Iwanj        Date: 24/04/02   Time: 17:37
 * Updated in $/xGate
 * remove licence info if licence invalid
 * 
 * *****************  Version 3  *****************
 * User: Iwanj        Date: 24/04/02   Time: 17:30
 * Updated in $/xGate
 * add  support  for disabled board using "XXX" for S/N
 * add additional info in licence file (product-ID and company name)
 * 
 * *****************  Version 2  *****************
 * User: Iwanj        Date: 23/04/02   Time: 13:23
 * Updated in $/xGate
 * initial version
 * 
 * *****************  Version 1  *****************
 * User: Iwanj        Date: 15/04/02   Time: 14:42
 * Created in $/xGate
 * initial version
*/

#include "stdafx.h"
#include <nb30.h>
#include <winsock.h>
#include <time.h>
#include "rsa\rsa.h"
#include "log.h"
#include "term.h"
#include "kernel.h"
#include "cfg.h"
#include "licence.h"

#define XGATE_LM_HTTP_ENABLED
#define DOWNLOADED_LICENCE_FILE	    "etc/xgate.tmp.lic"
#define DEFAULT_LICENSE_URL	    "http://80.74.225.117/lm/lmrequest.aspx"

static char PK[] =  "AA90A7C26672265F42C935572FAEDBA2A6A76BB720F69197094CBEC72E7301C4B85"
		    "FFAD6CAD5E523EF57715CD34E6BE5023B567A7DD42C5CC9ED8FBE8CBAA8EF1F0DD36393265ADA372"
		    "A247CA15DFB5AE5064DE4205D5570132CB9FE3D0CCCAF78CF3EBD2BDEBC4345198C7E3B33C9D882B"
		    "B5BAE47E3E1186A0B0C54F15C7DF57O2F791347334BC4F876ABB0CB8CD2E1205E6072F63D58BD01A"
		    "5A1B8470F179F6A25B89F2B0678E631B2EB4412D27C9BA931FE0C7B9947411DF34B350CFC2D2BADF"
		    "71D6CEDD225D0D5BA9EB8240F8B20856753907FDCA458A0389867DE77DE153F21AEA59A246701B2A"
		    "D68963221A736A3E9C81FD1EBEFBB18B0B8F1302A956623D";


LicenceManager* LicenceManager::instance_ = 0;

static const char __modname__[] = "LM";
#define THISMODULE  __modname__
#define NEWLINE	    "\n"
#define EVAL_TIME   3*60    
#define SIGNATURE   "000003FACE0F"
#define SIG_LEN	    6

#define NBVAL(x)    (x>='A'?x-'A'+10:x-'0')

inline void HEX_DECODE(const char *s,unsigned char *out)
{
    int nb1,nb2;
    while (*s) {
	nb1 = *s++;
	nb2 = *s++;
	*out++ = NBVAL(nb1)<<4 | NBVAL(nb2);
    }
}


LicenceManager::LicenceManager():    
    //ServiceProvider(module,attach_task,poll)
    ServiceProvider(THISMODULE,false,true), 
    cfgTree_(0),
    //fg_len_(0),
    running_(false),
    //total_fgprint_(0),
    mode_(Lm_Evaluation)
{
    memset(licence_file_,0,MAX_PATH);
    memset(licence_url_,0,MAX_PATH);
    memset(product_id_,0,HASH_LEN+1);
    memset(company_,0,NAME_LEN);

    //by default is from file
    http_licence_ = false;
    /**
	Allocate memory page for fingerprint buffer
    */
    //SYSTEM_INFO si;
    //GetSystemInfo(&si);
    //int fgps = (LICENCE_BUFSIZE/si.dwPageSize+1)*si.dwPageSize;
    //int rqps = ((2*LICENCE_BUFSIZE+1)/si.dwPageSize+1)*si.dwPageSize;
 
    //fgprint_ = (unsigned char*)VirtualAlloc(NULL,fgps,MEM_COMMIT,PAGE_READWRITE);
    //request_ = (char*)VirtualAlloc(NULL,rqps,MEM_COMMIT,PAGE_READWRITE);


    /* Fingerprint Signature, must be in the beginning of buffer */
    unsigned char sig[] = {0xFA,0xCE,0x0F};
    RegisterSerialNum(Fg_Signature,0,(char*)sig,sizeof(sig));
    RegisterSerialNum(Fg_Product,0, "xGate", strlen("xGate"));

    /**
	reserve 1 byte after signature for total number of fingerprint
	buffer format will be like this:

	+---------+-------------+--------+--------+----+--------+
	|FgSig    |Total-Fgprint|Fgprint1|Fgprint2|....|FgprintN|
	+---------+-------------+--------+--------+----+--------+

	Fgprint:
	+----+-------+--------+-----+
	|Tag | Minor | Length | S/N |
	+----+-------+--------+-----+

    */

    //fg_len_++;
    RegisterHostnameAndIpAddress();
    
}

LicenceManager::~LicenceManager()
{
    //VirtualFree(fgprint_,0,MEM_RELEASE);
    //VirtualFree(request_,0,MEM_RELEASE);
}

LicenceManager* LicenceManager::Create()
{
    if (instance_==0)
	instance_ = new LicenceManager;

    return instance_;
}

LicenceManager* LicenceManager::Instance()
{
    return instance_;
}

bool LicenceManager::Load()
{
    /**
	Create config tree 
    */
    cfgTree_ = new Cfg_Tree_Group("licence-manager",
				    "Licence Manager",
				    Kernel::Instance()->RootConfig());
    Term_Cmd* cmd = 0;

    cmd = new Term_Cmd (this, 
			Term_Cmd_LM_Config, 
			"licence-manager", 
			"Configure Licence", 
			PrivilegeLevelAdmin, 
			TermModeConfig, 
			false);
    RegisterTermCmd(cmd, Kernel::Instance()->RootConfig());

    cmd = new Term_Cmd(this,
			Term_Cmd_LM_ShowRequest,
			"create-request",
			"Generate licence request data",
			PrivilegeLevelAdmin,
			TermModeConfig,
			false);
    RegisterTermCmd(cmd,cfgTree_);

    cmd = new Term_Cmd(this,
			Term_Cmd_LM_ExportRequest,
			"export-request",
			"Export licence request data to file",
			PrivilegeLevelAdmin,
			TermModeConfig,
			false);
    cmd->Add(new Term_Cmd_Arg("value",CVariant(""),CVariantValidator("","Filename")));
    RegisterTermCmd(cmd,cfgTree_);

    cmd = new Term_Cmd(this,
		       Term_Cmd_LM_Reload,
		       "reload",
		       "Reload license from either local file or an URL",
		       PrivilegeLevelAdmin,
		       TermModeConfig,
		       false);
    RegisterTermCmd(cmd, cfgTree_);

    cmd = new Term_Cmd(this,
			Term_Cmd_LM_LicenceFile,
			"licence",
			"Configure licence file name and directory",
			PrivilegeLevelAdmin,
			TermModeConfig,
			false);


#ifndef XGATE_LM_HTTP_ENABLED

    cmd->Add(new Term_Cmd_Arg("value",CVariant(""),CVariantValidator("","Filename")));

#else	//XGATE_LM_HTTP_ENABLED

    /*** jnt: disable file ***
    CVariantChoiceValidator liVdtor("file", "License file");
    liVdtor.Add("url", "License URL i.e http://switchlab.ltd:888/lm.asp");
    ***/
    CVariantChoiceValidator liVdtor("url", "License URL i.e http://switchlab.ltd:888/lm.asp");

    cmd->Add(new Term_Cmd_Arg("value",CVariant(""),liVdtor));

    /*** jnt: disable file ***
    cmd->AddDynamic (1, 0, CVariant("file"),
		    new Term_Cmd_Arg("filename", 
		    CVariant("<STRING>"), 
		    CVariantValidator(CVariant(""), "Filename"),
		    TermArgRequired));
    ****/

    cmd->AddDynamic (1, 0, CVariant("url"),
		    new Term_Cmd_Arg("urlname", 
		    CVariant("<STRING>"), 
		    CVariantValidator(CVariant(""), "URL Address i.e http://switchlab.ltd:888/lm.asp"),
		    TermArgRequired));

#endif //XGATE_LM_HTTP_ENABLED

    RegisterTermCmd(cmd,cfgTree_);

    cfgTree_->RegisterShowHandler(this);
    Kernel::Instance()->RootConfig()->RegisterShowHandler(this);

    return true;
}

/*
bool LicenceManager::CommitConfiguration()
{
    // no more registration starting from this point 
    loaded_ = true;
    GenerateLicenceRequest();
    ProtectBuffer();

    if (VerifyLicence()!=0)
	CLog::Warning(THISMODULE,"Invalid licence file");

    return true;
}
*/

bool LicenceManager::CommitConfiguration()
{
    return true;
}

bool LicenceManager::KernelCommitConfiguration()
{
    /**
	Register default fingerprint
    */
    //RegisterDisk0SerialNum();

    /* simulate disabled disk */
    //RegisterSerialNum(Fg_Disk,0,"XXXX",4);

    // simulate adding new device
    //RegisterSerialNum(Fg_Disk,1,"ABCD",4);

    RegisterMacAddress();


    // Set default. licence_url_
    if (!http_licence_ && !*licence_file_) {
	http_licence_ = true;
	strcpy(licence_url_, DEFAULT_LICENSE_URL);
    }
    

    running_ = true;

#ifdef XGATE_LM_HTTP_ENABLED

    if (IsHttpLicence()) {
	if (DownloadLicenceFile(NULL, licence_url_) != 0) {
	    //If download is failed, use local file

	    String basedir = Kernel::Instance()->GetBaseDirectory();
	    basedir += DOWNLOADED_LICENCE_FILE;	

	    strcpy(licence_file_,basedir.c_str());
	}
    }

#endif	//XGATE_LM_HTTP_ENABLED

    if (VerifyLicence()!=0)
	CLog::Warning(THISMODULE,"Valid licence file hasn't been configured for this system");

    time(&time0_);
    return true;
}

bool LicenceManager::Unload()
{
    delete cfgTree_;

    FingerPrintListIter fi;
    for(fi=fgkey_.begin();fi!=fgkey_.end();++fi)
	delete *fi;
    fgkey_.clear();

    for(fi=fgsys_.begin();fi!=fgsys_.end();++fi)
	delete *fi;
    fgsys_.clear();
    
    return true;
}

void LicenceManager::HandleEvents()
{
    /**
    time limit for evaluation mode

    static shutting_down = false;

    if (mode_==Lm_Evaluation && !shutting_down) {
	long t1;
	time(&t1);

	if (t1-time0_>EVAL_TIME) {
	    shutting_down = true;

	    // evaluation time expired, shuting down
	    CLog::Warning(THISMODULE,"Shutting down the system. Evaluation time expired.");
	    Kernel::Instance()->Shutdown(KShutdown::HANGUP);
	}
    }
    */


    /* test virtual protection
    
    long t1;
    time(&t1);
    if (t1-time0_>60) {
	unsigned char f = fgprint_[LICENCE_BUFSIZE];

	char r = request_[LICENCE_BUFSIZE];

	request_[LICENCE_BUFSIZE] = 'F';
    }
    */   

    /* test disable, enable and replace device 
    
    long t1;
    time(&t1);
    static bool ena=false,dis=false,rep=false;

    // disabled
    if (t1-time0_==30 && !dis) {
	dis = true;
	CLog::Info(THISMODULE,"Disabling Disk0");
	RegisterSerialNum(Fg_Disk,0,"XXXX",4);
    }

    // enabled
    if (t1-time0_==45 && !ena) {
	ena = true;
	CLog::Info(THISMODULE,"Enabling Disk0");
	RegisterDisk0SerialNum();	
    }

    // replaced
    if (t1-time0_==60 && !rep) {
	rep = true;
	unsigned long serno = 0xBEEFCAFE;
	CLog::Info(THISMODULE,"Replacing Disk0");
	RegisterSerialNum(Fg_Disk,0,(char*)&serno,sizeof(unsigned long));
    }
    */
}

STATUS LicenceManager::OnShowConfig(
			const Cfg_Tree_Entry*,
			String& output,
			const String& indent,
			int indent_size) const
{
    output += indent+"licence-manager"+NEWLINE;

#ifndef XGATE_LM_HTTP_ENABLED

    if (*licence_file_)
	output += indent+" licence "+licence_file_+NEWLINE;

#else	//XGATE_LM_HTTP_ENABLED

    if (IsHttpLicence()==true) {
	if (*licence_url_)
	    output += indent+" licence url "+licence_url_+NEWLINE;
    } else {
	if (*licence_file_)
	    output += indent+" licence file "+licence_file_+NEWLINE;
    }

#endif	//XGATE_LM_HTTP_ENABLED

    output += indent+" top"+NEWLINE+NEWLINE;

    return OK;
}

bool LicenceManager::OnShowStatus(String& output)
{
    output += String("  Product ID: ")+product_id_+NEWLINE;
    output += String("  Registered to: ")+company_+NEWLINE;
    output += String("  Licence mode: ")+(mode_==Lm_NodeLocked?"node-locked":"evaluation")+NEWLINE;
    if (IsHttpLicence()==true) {
	output += String("  Licence URL: ") + (*licence_url_ ? licence_url_ : "(empty)") + NEWLINE;
    } else {
	output += String("  Licence file: ") + (*licence_file_ ? licence_file_ : "(empty)") + NEWLINE;
    }

    return true;
}

void LicenceManager::SendErrorMessage(Term_Session* session,const char* msg)
{
    session->SendErrorf(msg);
}

void LicenceManager::ReloadLicense()
{
    String file, url;

#ifndef XGATE_LM_HTTP_ENABLED

    if (*licence_file_) {
	file = licence_file_;
    } 

#else	//XGATE_LM_HTTP_ENABLED

    if (IsHttpLicence()==true) {
	if (*licence_url_) {
	    url = licence_url_;
	} 
    } else {
	if (*licence_file_) {
	    file = licence_file_;
	} 
    }

#endif	//XGATE_LM_HTTP_ENABLED

    if (url.size()) {
	char *tmp_str = strdup(url.c_str());

	if (tmp_str && DownloadLicenceFile(NULL, tmp_str) != 0) {
	    //If download is failed, use local file
	    
	}
	
	if (tmp_str) free(tmp_str);
	

	file = Kernel::Instance()->GetBaseDirectory();
	file += DOWNLOADED_LICENCE_FILE;	
    }

    if (file.size()) {

	strcpy(licence_file_, file.c_str());
	VerifyLicence();

    } else {
	mode_ = Lm_Evaluation;
    }
}

/**
    called before CommitConfiguration
*/
void LicenceManager::DoTermCmd(Term_Session* session,bool no,Term_Cmd* cmd)
{
    CVariant v;
    if (cmd->GetArgCount() > 0)
	cmd->GetArg((size_t)0)->GetValue(v);

    switch(cmd->GetId())
    {
    case Term_Cmd_LM_Reload:
	ReloadLicense();
	break;

    case Term_Cmd_LM_Config:
	session->SetRoot(cfgTree_);
	break;

    case Term_Cmd_LM_ShowRequest:
	//session->Sendf(request_);
	session->Sendf("Begin request:\n%s\nEnd request\n", CreateRequest());
	break;

    case Term_Cmd_LM_ExportRequest:
	if (ExportRequest(v.ToString().c_str())!=0)
	    SendErrorMessage(session,"Cannot create licence request file" NEWLINE);
	break;

    case Term_Cmd_LM_LicenceFile:

#ifndef XGATE_LM_HTTP_ENABLED
	
	strcpy(licence_file_,v.ToString().c_str());

	if (running_ && VerifyLicence()!=0)
	    SendErrorMessage(session,"Invalid licence file" NEWLINE);

#else	//XGATE_LM_HTTP_ENABLED

	{
	    IString arg = "";
	    String arg2 = "";

	    cmd->GetArg((size_t)0)->GetValue(v);
	    arg = v.ToString();

	    cmd->GetArg((size_t)1)->GetValue(v);
	    arg2 = v.ToString();

	    if (arg == "file") {
	    //strcpy(licence_file_,v.ToString().c_str());
		strcpy(licence_file_,arg2.c_str());
		http_licence_ = false;

		if (running_ && VerifyLicence()!=0)
		    SendErrorMessage(session,"Invalid licence file" NEWLINE);

	    } else if (arg == "url") {

		bool activate_now = true, save_url = true;

		if (session->GetStream().IsInteractive()) {
		    activate_now = session->PromptYesNo(false, "Do you want to activate this license now? (y/n) ");
		    if (!activate_now) {
			save_url = session->PromptYesNo(true, "Save this license URL? (y/n) ");
			if (!save_url)
			    return;
		    }
		}

		if (save_url) {
		    strcpy(licence_url_, arg2.c_str());
		    http_licence_ = true;
		}

		if (running_ && activate_now) {
		    char *tmp_str = strdup(arg2.c_str());

		    if (tmp_str && DownloadLicenceFile(session, tmp_str) != 0) {
			//If download is failed, use local file

			String basedir = Kernel::Instance()->GetBaseDirectory();
			basedir += DOWNLOADED_LICENCE_FILE;	

			strcpy(licence_file_,basedir.c_str());
		    }

		    if (tmp_str) free(tmp_str);

		    if (VerifyLicence()!=0) {
			SendErrorMessage(session,"Invalid licence file from URL" NEWLINE);
			CLog::Warning(THISMODULE, "Invalid licence file from URL");
		    }
		}

	    }
	}

#endif	//XGATE_LM_HTTP_ENABLED

	break;
    }
}

Cfg_Tree_Group* LicenceManager::GetConfigTree()
{
    return cfgTree_;
}

/*
void LicenceManager::RegisterSerialNum(FingerPrintId fg,const char* data,int len)
{
    static char fgminor[Fg_Unknown+1] = {0,0};

    if (loaded_) {
	CLog::Warning(THISMODULE,"Unable to register, service already loaded");
	return;
    }

    if (fg<Fg_Signature || fg>Fg_Unknown) {
	CLog::Warning(THISMODULE,"Unrecognized FingerPrintId %d",fg);
	return;
    }

    if (len>MAX_SN_LEN) {
	CLog::Warning(THISMODULE,"Serial number too long %d bytes",len);
	return;
    }

    // add 3 for device-type,device-minor and length 
    if (fg_len_+len+3>=LICENCE_BUFSIZE) {
	CLog::Warning(THISMODULE,"Licence buffer overflow");
	return;
    }

    // inc total number of fingerprint 
    ++total_fgprint_;

    fgprint_[fg_len_++] = fg;
    fgprint_[fg_len_++] = fgminor[fg]++;
    fgprint_[fg_len_++] = len;

    memcpy(&fgprint_[fg_len_],data,len);
    fg_len_ += len;
}
*/

/**
    the new register function can be called anytime while the system is running
    validation has to be done immediately after S/N registration by comparing
    S/N value against S/N built from licence key
*/
void LicenceManager::RegisterSerialNum(FingerPrintId fg,char minor,const char* data,int len)
{
    static char *TAG[] = {
	"Signature",
	"Processor",
	"Disk",
	"Ethernet",
	"Aculab",
	"Audiocodes",
	"Dialogic",
	"Atm",
	"Mapletree",
	"Hostname",
	"IpAddress",
	"Product"
	"Prosody",
	"PowerAccess"
	"Unknown",
    };

    if (fg<Fg_Signature || fg>Fg_Unknown) {
	CLog::Warning(THISMODULE,"Unrecognized FingerPrintId %d",fg);
	return;
    }

    if (len>MAX_SN_LEN) {
	CLog::Warning(THISMODULE,"Serial number too long (%d bytes)",len);
	return;
    }

    /* find */
    FingerPrintListIter fi;
    for(fi=fgsys_.begin();fi!=fgsys_.end();++fi) {
	FingerPrint *fp = *fi;
	if (fp->tag_==fg && fp->min_==minor)
	    break;
    }

    /* insert or update list */
    if (fi!=fgsys_.end()) {
	FingerPrint *fp = *fi;
	if (memcmp(fp->val_,data,fp->len_)!=0) {
	    // different S/N registered, modify existing entry
	    memcpy(fp->val_,data,len);
	    fp->len_ = len;
	}
    }
    else
	fgsys_.push_back(new FingerPrint(fg,minor,(unsigned char*)data,len));

    /* we've got new S/N to check */
    if (CompareFingerPrint(fg,minor,(unsigned char*)data,len)!=0) {
	CLog::Warning(THISMODULE,"Unregistered device (%s%d) detected",TAG[fg],minor);

	// reset to evaluation mode
	memset(product_id_,0,HASH_LEN+1);
	memset(company_,0,NAME_LEN);
	mode_ = Lm_Evaluation;
    }
}

/*
void LicenceManager::GenerateLicenceRequest()
{
    static char hex[] = "0123456789ABCDEF";
    int i,c,p=0;

    patch total number of fingerprint in the buffer 
    this will exclude the signature

    fgprint_[SIG_LEN] = total_fgprint_-1;

    for(i=0;i<fg_len_;++i) {
	c = fgprint_[i];
	request_[p++] = hex[(c>>4) & 0xf];
	request_[p++] = hex[c & 0xf];
    }
    request_[p] = '\0';
}


void LicenceManager::ProtectBuffer() 
{
    DWORD op;
    int r1 = VirtualProtect(fgprint_,LICENCE_BUFSIZE,PAGE_NOACCESS,&op);
    int r2 = VirtualProtect(request_,2*LICENCE_BUFSIZE+1,PAGE_READONLY,&op);
}
*/

const char* LicenceManager::CreateRequest()
{
    static char request[2*LICENCE_BUFSIZE+1];
    static char hex[] = "0123456789ABCDEF";

    memset(request,0,2*LICENCE_BUFSIZE+1);
/*
    patch total number of fingerprint in the buffer 
    this will exclude the signature
    
    fgprint_[SIG_LEN] = total_fgprint_-1;

    int i;
    for(i=0;i<fg_len_;++i) {
	c = fgprint_[i];
	request_[p++] = hex[(c>>4) & 0xf];
	request_[p++] = hex[c & 0xf];
    }
    request_[p] = '\0';
*/
    FingerPrintListIter fi;
    int p = 0;
    unsigned char totsig = fgsys_.size()-1;

    for(fi=fgsys_.begin();fi!=fgsys_.end();++fi) {
	FingerPrint *fg = *fi;

	// tag
	request[p++] = hex[(fg->tag_ >> 4) & 0xf];
	request[p++] = hex[fg->tag_ & 0xf];

	// minor
	request[p++] = hex[(fg->min_ >> 4) & 0xf];
	request[p++] = hex[fg->min_ & 0xf];

	// length
	request[p++] = hex[(fg->len_ >> 4) & 0xf];
	request[p++] = hex[fg->len_ & 0xf];

	// value
	int i;
	for(i=0;i<fg->len_;++i) {
	    request[p++] = hex[(fg->val_[i] >> 4) & 0xf];
	    request[p++] = hex[fg->val_[i] & 0xf];
	}

	if (fg->tag_==Fg_Signature) {
	    request[p++] = hex[(totsig >> 4) & 0xf];
	    request[p++] = hex[totsig & 0xf];
	}
    }

    request[p] = '\0';

    return request;
}

int LicenceManager::ExportRequest(const char* fn)
{
    FILE* fp = fopen(fn,"wt");
    if (!fp) {
	CLog::Warning(THISMODULE,"Error creating licence request file %s",fn);
	return -1;
    }

    const char *request = CreateRequest();

    int reqsz = strlen(request);
    int nbyte = fwrite(request,1,reqsz,fp);
    if (nbyte!=reqsz) {
	CLog::Warning(THISMODULE,"Error writing licence request file");
	fclose(fp);
	return -1;
    }

    fclose(fp);
    return 0;
}

void LicenceManager::TrimSpace(char *s)
{
    char *d = s;

    while (*s) {
	if (isspace(*s)) {
	    ++s;
	    continue;
	}

	*d++ = *s++;
    }

    *d = '\0';
}

int LicenceManager::ReadLicenceFile()
{
    /* reset licence info */
    memset(product_id_,0,HASH_LEN+1);
    memset(company_,0,NAME_LEN);

    /**
	load licence key from file 
    */
    if (strlen(licence_file_)==0) 
	return -1;

    FILE *fp = fopen(licence_file_,"rb");
    if (!fp) {
	CLog::Warning(THISMODULE,"Error opening licence file %s",licence_file_);
	return -1;
    }

    fseek(fp,0L,SEEK_END);
    long fsz = ftell(fp);
    fseek(fp,0L,SEEK_SET);
    char *licence = (char*)malloc(fsz+1);
    if (licence==NULL) {
	CLog::Warning(THISMODULE,"Cannot allocate buffer for reading licence file");
	fclose(fp);
	unlink(licence_file_); //*** jnt: disable file ***
	return -1;
    }

    long frd = 0;
    while (!feof(fp)) {
	long n = fread(&licence[frd],1,64,fp);
	frd += n;
    }
    licence[frd] = '\0';

    if (frd!=fsz) {
	CLog::Warning(THISMODULE,"Error during reading licence file");
	free(licence);
	fclose(fp);
	unlink(licence_file_); //*** jnt: disable file ***
	return -1;
    }

    fclose(fp);
    unlink(licence_file_); //*** jnt: disable file ***

    TrimSpace(licence);

    if (strlen(licence)==0) {
	free(licence);
	return -1;
    }

    /** 
	decrypt and build fingerprint list
    */
    const int CLEAR_BLOCK_SIZE = 129;

    /*	buffer for cleartext mustbe large enough to hold data being processed 
	it depends on the block-size not the size of cipher-text  */
    int clrsiz = (strlen(licence)/CLEAR_BLOCK_SIZE+1)*CLEAR_BLOCK_SIZE;
    char* clrtxt = (char*)malloc(clrsiz);
    if (clrtxt==NULL) {
	free(licence);
	return -1;
    }

    memset(clrtxt,0,clrsiz);

    if (rsa_decrypt(PK,licence,clrtxt)!=0) 
	goto error_1;

    /**
    licence format (hex ascii) after decryption
    +---------+--------------+-------------+
    | prod-id | company-name | orig-request|
    +---------+--------------+-------------+
      HASH_LEN   2*NAME_LEN          n
    */
    if (strlen(clrtxt)<HASH_LEN+2*NAME_LEN+2*SIG_LEN+1)
	goto error_1;

    memcpy(product_id_,clrtxt,HASH_LEN);
    product_id_[HASH_LEN] = '\0';

    char hex[2*NAME_LEN+1];
    memcpy(hex,&clrtxt[HASH_LEN],2*NAME_LEN);
    hex[2*NAME_LEN] = '\0';

    HEX_DECODE(hex,(unsigned char*)company_);

     if (strncmp(&clrtxt[HASH_LEN+2*NAME_LEN],SIGNATURE,strlen(SIGNATURE))!=0) 
	goto error_1;

    if (CreateListFromLicence(&clrtxt[HASH_LEN+2*NAME_LEN])!=0)
	 goto error_2;

    free(licence);
    free(clrtxt);
    return 0;

error_2:
    memset(product_id_,0,HASH_LEN+1);
    memset(company_,0,NAME_LEN);

error_1:
    free(licence);
    free(clrtxt);

    return -1;
}

int LicenceManager::CreateListFromLicence(const char *s)
{
    /* destroy current list */
    FingerPrintListIter fi;
    for(fi=fgkey_.begin();fi!=fgkey_.end();++fi)
	delete *fi;
    fgkey_.clear();


    /* point to fg_len */
    s += 2*SIG_LEN;

    /* get fg_len */
    int nb1 = *s++;
    int nb2 = *s++;
    int fglen = NBVAL(nb1)<<4 | NBVAL(nb2);

    /* iterate over fingerprint string */
    while (*s) {
	/* get tag */
	nb1 = *s++;
	nb2 = *s++;
	int tag = NBVAL(nb1)<<4 | NBVAL(nb2);

	if (tag<Fg_Signature || tag>=Fg_Unknown)  {
	    CLog::Info(THISMODULE, "Unknown tag %d ignored", tag);
	}

	/* get minor */
	nb1 = *s++;
	nb2 = *s++;
	int min = NBVAL(nb1)<<4 | NBVAL(nb2);

	/* get length */
	nb1 = *s++;
	nb2 = *s++;
	int len = NBVAL(nb1)<<4 | NBVAL(nb2);

	/* get s/n value, convert from hex ascii to byte value */
	if (len>MAX_SN_LEN || len < 0) 
	    goto create_error_1;

	unsigned char tmp[MAX_SN_LEN];
	int i;
	for(i=0;i<len;++i) {
	    nb1 = *s++;
	    nb2 = *s++;
	    tmp[i] = NBVAL(nb1)<<4 | NBVAL(nb2);
	}

	fgkey_.push_back(new FingerPrint(FingerPrintId(tag),min,(unsigned char*)tmp,len));
    }

    if (fglen!=fgkey_.size()) 
	goto create_error_1;

    return 0;

create_error_1:
    /* abort */
    for(fi=fgkey_.begin();fi!=fgkey_.end();++fi)
	delete *fi;
    fgkey_.clear();

    return -1;
}

int LicenceManager::VerifyLicence()
{
    mode_ = Lm_Evaluation;

    if (ReadLicenceFile()!=0)
	return -1;

    if (CompareFingerPrint()!=0) {
	memset(product_id_,0,HASH_LEN+1);
	memset(company_,0,NAME_LEN);

	return -1;
    }

    mode_ = Lm_NodeLocked;
    return 0;
}

/*
int LicenceManager::CompareFingerPrint()
{
    FingerPrintListIter key_i;

    for(key_i=fgkey_.begin();key_i!=fgkey_.end();++key_i) {
	int found = 0;
	FingerPrint *fgk = *key_i;

	FingerPrintListIter sys_i;
	FingerPrint *fgs = NULL;
	for(sys_i=fgsys_.begin();sys_i!=fgsys_.end();++sys_i) {
	    fgs = *sys_i;

	    if (fgk->tag_ == fgs->tag_ && fgk->min_==fgs->min_) {
		found = 1;
		break;
	    }
	}

	if (found && fgs) {
	    if (memcmp(fgk->val_,fgs->val_,fgk->len_)!=0 && 
		memcmp(fgs->val_,SN_UNKNOWN,fgs->len_)!=0) {

		return -1;
	    }
	}
	else {

	    return -1;
	}

    }

    return 0;
}
*/

int LicenceManager::CompareFingerPrint()
{
    FingerPrintListIter sys_i;
    for(sys_i=fgsys_.begin();sys_i!=fgsys_.end();++sys_i) {
	int found = 0;
	FingerPrint *fgs = *sys_i;

	if (fgs->tag_==Fg_Signature)
	    continue;

	FingerPrintListIter key_i;
	FingerPrint *fgk = NULL;
	for(key_i=fgkey_.begin();key_i!=fgkey_.end();++key_i) {
	    fgk = *key_i;

	    if (fgk->tag_==fgs->tag_ && fgk->min_==fgs->min_) {
		found  = 1;
		break;
	    }
	}

	if (found && fgk) {
	    if (memcmp(fgk->val_,fgs->val_,fgk->len_)!=0 && 
		memcmp(fgs->val_,SN_UNKNOWN,fgs->len_)!=0) {

		return -1;
	    }
	}
	else {
	    return -1;
	}
    }

    return 0;
}


int LicenceManager::CompareFingerPrint(FingerPrintId tag,char minor,const unsigned char *data,int len)
{
    /* this has to be the case when the licence hasn't been loaded */
    if (fgkey_.empty())
	return 0;

    FingerPrintListIter key_i;
    for(key_i=fgkey_.begin();key_i!=fgkey_.end();++key_i) {

	if ((*key_i)->tag_ == tag && (*key_i)->min_==minor) 
	    break;
    }

    if (key_i==fgkey_.end())
	return -1;

    FingerPrint *fgk = *key_i;
    if (memcmp(fgk->val_,data,fgk->len_)!=0 && 
	memcmp(data,SN_UNKNOWN,len)!=0) {

	return -1;
    }


    return 0;
}

bool LicenceManager::IsHttpLicence() const
{
    return http_licence_;
}

#if 1	    //** jnt reactivated
/////////////////////////////// OLD CODE WITH WININET /////////////////////////////////

int LicenceManager::URLtoAddressPort(char *url, char *addr, INTERNET_PORT *port, char *page)
{
    char *tmp_addr;
    char *tport;
    char *tpath;

    if (strnicmp("http://", url, 7)==0)
	tmp_addr = (char *)&url[7];
    else
	tmp_addr = (char *)url;

    tport = strchr(tmp_addr, ':');

    if (tport) {
	*tport = '\0';
	tpath = strchr(tport+1, '/');
	if (tpath) {
	    *tpath = '\0';
	    strcpy(page, tpath+1);
	}

	*port = atol(tport+1);
	strcpy(addr, tmp_addr);
    } else {
	tpath = strchr(tmp_addr, '/');
	if (tpath) {
	    *tpath = '\0';
	    strcpy(page, tpath+1);
	}

	strcpy(addr, tmp_addr);
    }

    return 0;
}

int LicenceManager::SaveDownloadBuffer(HINTERNET hin)
{
    LPVOID lpBufMsg, lpBuf;
    DWORD max_len, len, total;
    bool result = false;
    FILE *fp = NULL;
    String basedir = Kernel::Instance()->GetBaseDirectory();
    basedir += DOWNLOADED_LICENCE_FILE;

    DWORD index = 0;
    unsigned response_length;
    DWORD len_size = sizeof(response_length);
    if (!HttpQueryInfo(
	    hin, 
	    HTTP_QUERY_CONTENT_LENGTH|HTTP_QUERY_FLAG_NUMBER, 
	    &response_length, &len_size, &index)) {

	int err = GetLastError();
	switch (err) {
	case ERROR_HTTP_HEADER_NOT_FOUND:
	    CLog::Warning(THISMODULE,"Content-Length not defined by %s", licence_url_);
	    response_length = 65536;
	    break;
	default:
	    CLog::Warning(THISMODULE,"Error %d while querying response length from %s", err, licence_url_);
	    return -1;
	}
    }

    lpBuf = malloc(response_length);
    if (lpBuf==NULL)
	return -1;

    max_len = response_length;
    total = 0;
    lpBufMsg = lpBuf;

    while (max_len && InternetReadFile(hin, lpBufMsg, max_len, &len)) {

	if (!len) {
	    //download completed
	    result = true;
	    break;
	}

	total += len;
	lpBufMsg = (LPVOID) ((char *)lpBufMsg + len);
	max_len -= len;
    }

    if (!result && (max_len == 0)) {
	result = true;
    }

    if (total)
	fp = fopen(basedir.c_str(), "wb");

    if (fp) {
	fwrite(lpBuf, sizeof(char), total, fp);
	fclose(fp);

	strcpy(licence_file_, basedir.c_str());
    } else {
	result = false;
    }

    free(lpBuf);

    if (result)
	return 0;
    else
	return -1;
}


#define METHOD_POST	1
#define METHOD_GET	2
#define WHICH_METHOD	METHOD_GET

int LicenceManager::DownloadLicenceFile(Term_Session* session, const char *const_url)
{
    char server_addr[64], server_page[256];
    INTERNET_PORT server_port = 80;
    HINTERNET hin = INVALID_HANDLE_VALUE;
    HINTERNET hcon = INVALID_HANDLE_VALUE;
    HINTERNET hdown = INVALID_HANDLE_VALUE;
    char request[2*LICENCE_BUFSIZE+1];
    BOOL rc;
    char http_status_code[20];
    DWORD http_status_code_length = sizeof(http_status_code);
    DWORD http_index;
    const char *http_post_header = "Content-Type: application/x-www-form-urlencoded";
    const char *accept_types[] = {"text/*", "*/*", NULL};

    //*** jnt: disable file ***
    String filepath = Kernel::Instance()->GetBaseDirectory();
    filepath += DOWNLOADED_LICENCE_FILE;
    unlink(filepath.c_str());

    hin = InternetOpen(Kernel::Instance()->GetVersion().c_str(), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL,
			INTERNET_FLAG_DONT_CACHE);

    if (hin == INVALID_HANDLE_VALUE) {
	if (session)
	    SendErrorMessage(session, "Cannot open HTTP handle for LM" NEWLINE);
	CLog::Warning(THISMODULE,"Cannot open HTTP handle for LM");

	return -1;
    }

    ZeroMemory(server_addr, 64 * sizeof(char));
    ZeroMemory(server_page, 256 * sizeof(char));

    do {
	char the_url[255];
	strcpy(the_url, const_url);//DG changed
	URLtoAddressPort(the_url, server_addr, &server_port, server_page);

    } while (0);


    hcon = InternetConnect(hin, server_addr, server_port, NULL, NULL, INTERNET_SERVICE_HTTP, 
			    INTERNET_FLAG_DONT_CACHE, NULL);

    if (hcon == INVALID_HANDLE_VALUE) {
	if (session)
	    SendErrorMessage(session, "Cannot open HTTP connection for LM" NEWLINE);
	CLog::Warning(THISMODULE,"Cannot open HTTP connection for LM, url=%s", const_url);

	goto download_error;
    }

#if WHICH_METHOD==METHOD_GET
    sprintf(request, "%s?PostRequestData=%s", server_page, CreateRequest());
    hdown = HttpOpenRequest(hcon, "GET", request, NULL/*"HTTP/1.1"*/, NULL, accept_types, 0, NULL);

    if (hdown == INVALID_HANDLE_VALUE) {
	printf("Cannot create HTTP request for LM" NEWLINE);
	if (session)
	    SendErrorMessage(session, "Cannot create HTTP request for LM" NEWLINE);
	CLog::Warning(THISMODULE,"Cannot create HTTP request for LM" );
	goto download_error;
    }

    if (HttpSendRequest(hdown, NULL, 0, (LPVOID)"", 0)==false) {
	if (session)
	    SendErrorMessage(session, "Error sending HTTP request for LM" NEWLINE);
	CLog::Warning(THISMODULE,"Error sending HTTP request for LM" );
	goto download_error;
    }

#elif WHICH_METHOD==METHOD_POST
    hdown = HttpOpenRequest(hcon, "POST", server_page, "HTTP/1.1", NULL, NULL, 0, NULL);

    if (hdown == INVALID_HANDLE_VALUE) {
	if (session)
	    SendErrorMessage(session, "Cannot create HTTP request for LM" NEWLINE);
	CLog::Warning(THISMODULE,"Cannot create HTTP request for LM");

	goto download_error;
    }

    DWORD req_len;
    req_len = sprintf(request, "PostRequestData=%s", CreateRequest());

    if (HttpSendRequest(hdown, http_post_header, strlen(http_post_header), 
		       (LPVOID)request, req_len)==false) {
	if (session)
	    SendErrorMessage(session, "Cannot send HTTP request for LM" NEWLINE);
	CLog::Warning(THISMODULE,"Cannot send HTTP request for LM");

	goto download_error;
    }
#else
# error "Which method?"
#endif

    http_index = 0;
    rc = HttpQueryInfo(hdown, HTTP_QUERY_STATUS_CODE, http_status_code, &http_status_code_length, &http_index);
    if (rc) {
	int status = atoi(http_status_code);
	if (status/100 != 2) {
	    if (session)
		session->SendErrorf("Status %s was returned from server" NEWLINE, http_status_code);
	    CLog::Warning(THISMODULE, "Status %s was returned from server", http_status_code);

	    // jnt *** display the received response
	    {
		DWORD index = 0;
		unsigned response_length = 0;
		DWORD len_size = sizeof(response_length);
		if (!HttpQueryInfo(
			hdown, 
			HTTP_QUERY_CONTENT_LENGTH|HTTP_QUERY_FLAG_NUMBER, 
			&response_length, &len_size, &index)) {

		    int err = GetLastError();
		    switch (err) {
		    case ERROR_HTTP_HEADER_NOT_FOUND:
			CLog::Warning(THISMODULE,"Content-Length not defined by %s", licence_url_);
			response_length = 65536;
			break;
		    default:
			CLog::Warning(THISMODULE,"Error %d while querying response length from %s", err, licence_url_);
			break;
		    }
		}

		if (response_length) {
		    char buf[65536];
		    DWORD max_len = min(response_length, sizeof(buf)-1);
		    unsigned total = 0;
		    DWORD len = 0;
		    char *pbuf = buf;

		    while (max_len && InternetReadFile(hdown, pbuf, max_len, &len)) {

			if (!len) {
			    //download completed
			    break;
			}

			total += len;
			pbuf = pbuf + len;
			max_len -= len;
		    }

		    if (len && (max_len == 0)) {
			len = 0;
		    }

		    if (len == 0) {
			buf[total] = 0;
			if (session)
			    session->SendErrorf("%s" NEWLINE, buf);
			CLog::Warning(THISMODULE, "%s", buf);
		    }
		}

	    }

	    goto download_error;
	}
    }

    if (SaveDownloadBuffer(hdown)) {
	if (session)
	    SendErrorMessage(session, "Cannot read and save URL for LM" NEWLINE);
	CLog::Warning(THISMODULE,"Cannot read and save URL for LM");

	goto download_error;
    }

    InternetCloseHandle(hdown);
    InternetCloseHandle(hcon);
    InternetCloseHandle(hin);

    return 0;

download_error:

    if (hdown != INVALID_HANDLE_VALUE) {
	InternetCloseHandle(hdown);
    }

    if (hcon != INVALID_HANDLE_VALUE) {
	InternetCloseHandle(hcon);
    }

    if (hin != INVALID_HANDLE_VALUE) {
	InternetCloseHandle(hin);
    }

    return -1;
}

#else	////////////////////// END OF DISABLED OLD CODE WITH WININET ///////////////////////


/*
 * New download license file.
 */
#include "tiny_http/http_lib.h"

int LicenceManager::DownloadLicenceFile(Term_Session* session, const char *const_url)
{
    char *url = (char*)malloc(1024);
    if (!url) {
	CLog::Error(THISMODULE, "No memory!");
	return -1;
    }

    //*** jnt: disable file ***
    String filepath = Kernel::Instance()->GetBaseDirectory();
    filepath += DOWNLOADED_LICENCE_FILE;
    unlink(filepath.c_str());

    sprintf(url, "%s?PostRequestData=%s", const_url, CreateRequest());


    int ret, length;
    char *data=NULL, *filename=NULL;
    char typebuf[128];
    http_struct *ht;
    int error = 0;
    

    ht = http_create(Kernel::Instance()->GetVersion().c_str(), 120);
    if (!ht) {
	CLog::Error(THISMODULE, "No memory!");
	free(url);
	return -1;
    }

    ret = http_parse_url(ht, url, &filename);
    if (ret < 0) {
	free(url);
	if (session)
	    SendErrorMessage(session, "Error parsing URL (invalid URL was given)." NEWLINE);
	CLog::Warning(THISMODULE, "Error parsing URL (invalid URL was given).");
	return -1;
    }

    ret = http_get(ht, filename, &data, &length, typebuf);
    
    if (ret/100 == 2) {
	if (session)
	    session->Sendf("Saving license (%d bytes)..." NEWLINE, length);
	CLog::Info(THISMODULE,"Saving license (%d bytes)...", length);

	CLog::Detail(THISMODULE, "Status %d, data length=%d", ret, length);

	FILE *fp = fopen(filepath.c_str(), "wb");
	if (fp) {
	    fwrite(data, sizeof(char), length, fp);
	    fclose(fp);

	    strcpy(licence_file_, filepath.c_str());

	} else {
	    if (session)
		SendErrorMessage(session, "Unable to write license file!" NEWLINE);
	    CLog::Warning(THISMODULE, "Unable to write license file!");
	    error = -1;
	}


    } else {
	if (session)
	    session->SendErrorf("Received HTTP error status %d from server" NEWLINE, ret);
	CLog::Warning(THISMODULE,"Received HTTP error status %d from server", ret);

	error = -1;
    }

    if (data) free(data);
    free(filename);
    free(url);
    http_destroy(ht);

    return error;
}

#endif


LicenceMode LicenceManager::GetCurrentLicenceMode() const
{
    return mode_;
}

void LicenceManager::RegisterHostnameAndIpAddress()
{
    char hostname[64];
    if (gethostname(hostname,sizeof(hostname)-1)==SOCKET_ERROR)
        return;

    RegisterSerialNum(Fg_Hostname,0, hostname,strlen(hostname));


    in_addr addr;
    addr.s_addr = inet_addr(hostname);
    char *ip_address = inet_ntoa(addr);
    if (addr.s_addr==INADDR_NONE) {
	hostent* phe = gethostbyname(hostname);
	if (phe) {
	    addr.s_addr = *((unsigned long*)phe->h_addr);
	    ip_address = inet_ntoa(addr);
	}
    }

    RegisterSerialNum(Fg_IpAddress,0, ip_address,strlen(ip_address));
}

/*
void LicenceManager::RegisterPentium3SerialNum()
{
    unsigned long serno[3];
    unsigned long t,m,b;
    int found_sn;

    _asm {
	pushfd
	pop eax		    // get EFLAGS into eax
	mov ebx,eax	    // keep a copy
	xor eax,0x200000    // toggle CPUID bit
	push eax
	popfd		    // set new EFLAGS
	pushfd
	pop eax		    // EFLAGS back into eax
			    // have we changed the ID bit?
	xor eax,ebx 
	je NO_SERIAL_NUM    // we could toggle the bit so CPUID
			    // is present
    
	mov eax,1
	cpuid		    // get processor features
	test edx,1<<18	    // check the serial number bit
	jz NO_SERIAL_NUM

	mov found_sn,1 
	jmp DONE

	NO_SERIAL_NUM:
	mov found_sn,0

	DONE:
    }

    if (!found_sn) 
	return;

    _asm {
	mov eax,1
	cpuid
	// top 32 bits are the processor
	// signature bits
	mov t,eax

	// A new CPUID code for the
	// Pentium III
	mov eax,3


	cpuid
	mov m,edx
	mov b,ecx

    }

    // copy the locals into the pointer variables passed in
    serno[0] = b;
    serno[1] = m;
    serno[2] = t;

#if 0
    CLog::Debug(THISMODULE,"Pentium III Serial Number: %04X-%04X-%04X-%04X-%04X-%04X\n",
	serno[0]&0xffff,
	serno[0]>>16,
	serno[1]&0xffff,
	serno[1]>>16,
	serno[2]&0xffff,
	serno[2]>>16
	);
#endif

    RegisterSerialNum(Fg_Processor,(char*)serno,sizeof(serno));
}
*/

void LicenceManager::RegisterDisk0SerialNum()
{
    unsigned long a,b,serno=0;
    if (!GetVolumeInformation("C:\\",NULL,0,&serno,&a,&b,NULL,0))
	return;

#if 0
    CLog::Debug(THISMODULE,"Disk Serial Number: %04X-%04X\n",serno>>16,serno&0xffff);
#endif

    RegisterSerialNum(Fg_Disk,0,(char*)&serno,sizeof(unsigned long));
}


void LicenceManager::RegisterMacAddress()
{
    typedef struct _ASTAT_
    {
	ADAPTER_STATUS adapt;
	NAME_BUFFER    NameBuff [30];
    } ASTAT,*PASTAT;

    ASTAT Adapter;

    NCB Ncb;
    UCHAR uRetCode;
    LANA_ENUM   lenum;
    int      i;

    memset(&Ncb,0,sizeof(Ncb));
    Ncb.ncb_command = NCBENUM;
    Ncb.ncb_buffer = (UCHAR *)&lenum;
    Ncb.ncb_length = sizeof(lenum);
    uRetCode = Netbios(&Ncb);

    for(i=0;i<lenum.length;i++) {
	memset(&Ncb,0,sizeof(Ncb) );
	Ncb.ncb_command = NCBRESET;
	Ncb.ncb_lana_num = lenum.lana[i];

	uRetCode = Netbios( &Ncb );

	memset(&Ncb,0,sizeof (Ncb) );
	Ncb.ncb_command = NCBASTAT;
	Ncb.ncb_lana_num = lenum.lana[i];

	strcpy((char*)Ncb.ncb_callname,  "*               " );
	Ncb.ncb_buffer = (unsigned char *) &Adapter;
	Ncb.ncb_length = sizeof(Adapter);

	uRetCode = Netbios( &Ncb );

	if ( uRetCode == 0 ) {

#if 0
	    CLog::Debug(THISMODULE,"MAC Address #%d: %02X%02X%02X-%02X%02X%02X\n",
		    lenum.lana[i],
		    Adapter.adapt.adapter_address[0],
		    Adapter.adapt.adapter_address[1],
		    Adapter.adapt.adapter_address[2],
		    Adapter.adapt.adapter_address[3],
		    Adapter.adapt.adapter_address[4],
		    Adapter.adapt.adapter_address[5]);
#endif

	    RegisterSerialNum(Fg_Ethernet,i,(char*)Adapter.adapt.adapter_address,sizeof(Adapter.adapt.adapter_address));
	}
    }
} 


