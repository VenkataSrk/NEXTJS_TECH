#include "ace/OS.h"
#include "ace/OS_NS_errno.h"
#include "ace/OS_NS_sys_time.h"
#include "ace/os_include/os_netdb.h"
#include "ClientService.h"
#include "ace/Process_Manager.h"

#include "ace/Log_Msg.h"
#include "ace/INET_Addr.h"
#include "ace/SOCK_Acceptor.h"
#include "ace/Reactor.h"
#include "ace/Acceptor.h"
#include "ace/Thread.h"
#include "ace/Synch.h"
#include "xglog.h"
#include "xGateMBCUtil.h"
#include "xGateMBCService.h"
#define  THISMODULE "MBC_CLIENT_SERVICE"
ClientService *pClntService;
ACE_thread_t *threadID;
ACE_hthread_t *threadHandles;
ACE_hthread_t *sfuThreadHandles;
int create_mbc_thread (void);
static void *sfuThread (void *arg); 

class Manager: public ACE_Process_Manager
{
    private:
        ACE_TCHAR strProgramName [64];
        ACE_HANDLE outputfd_;
        ACE_TCHAR programName_[256];


    public:
        Manager (const ACE_TCHAR * strProgramName)
        {
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT ("Manager :: Manager ()\n")));
            ACE_OS :: memset (this-> strProgramName, 0, sizeof (this-> strProgramName));
            ACE_OS :: strcpy (this-> strProgramName, strProgramName);
        }

        int doWork (int sfuId)
        {
            int nReturn = 0;
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT ("Manager :: doWork ()\n")));

            //declare a ACE_Process_Options object options;
            ACE_Process_Options options;
            //int sfuId = xGateMBCUtil::getSfuId();
	    
            options.command_line ("%s %s %d","./mbc","sfu",sfuId);
            //create an new process with options;
            pid_t pid = this-> spawn (options);
            if (pid <0)
            {
                ACE_DEBUG ((LM_ERROR, ACE_TEXT ("failed to create sub process [% d]:% p\n"), pid, ACE_TEXT ("spawn")));
                return -1;
            }
            else
            {
                xGateMBCUtil::addToMgSfuConnectionMap(sfuId, pid);
            }

            ACE_DEBUG ((LM_DEBUG, ACE_TEXT ("spawn child:% d\n"), pid));
            //wait for my child to exit
            nReturn = this-> wait ();
            if (nReturn <0)
            {
                ACE_DEBUG ((LM_ERROR, ACE_TEXT ("failed to wait [% d]\n"), nReturn, ACE_TEXT ("wait")));
                return -2;
            }
            //this->dumpRun ();
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT ("exited child success:% d\n"), pid));

            return 0;
        }//end of the dowork ()
       int setStdHandles (ACE_Process_Options &options)
        {
            ACE_TRACE ("Manager::setStdHandles");

            ACE_OS::unlink ("output.dat");
            this->outputfd_ =
                ACE_OS::open ("output.dat", O_RDWR | O_CREAT);
            return options.set_handles
                (ACE_STDIN, ACE_STDOUT, this->outputfd_);
        }
        int setEnvVariable (ACE_Process_Options &options)
        {
            ACE_TRACE ("Manager::setEnvVariables");
            return options.setenv
                (ACE_TEXT ("PRIVATE_VAR=/that/seems/to/be/it"));
        }
        int setUserID (ACE_Process_Options &options)
        {
            ACE_TRACE ("Manager::setUserID");
            passwd* pw = ACE_OS::getpwnam ("nobody");
            if (pw == 0)
                return -1;
            options.seteuid (pw->pw_uid);
            return 0;
        }

        //Before creating a new child process, the ACE_Process :: spawn () method will automatically call the process object
        //ACE_Process :: prepare () method to do some pre-processing
        int prepare (ACE_Process_Options & options)
        {
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT ("ACE_Process :: prepare\n")));
            options.command_line (ACE_TEXT ("%s 1"), this->programName_);
            if (this->setStdHandles (options) == -1 ||
                    this->setEnvVariable (options) == -1)
                return -1;
#if !defined (ACE_WIN32) && !defined (ACE_LACKS_PWD_FUNCTIONS)
            return this->setUserID (options);
#else
            return 0;
#endif

            return 0;
        }
 

        //After the CreatProcess function (windows platform) is called, this method will be immediately in the parent process
        //Called back
        void parent (pid_t child)
        {
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT ("parent (child):% d\n"), child));
        }

        //On the windows platform, the child function will not be called because on the windows platform
        //The concept of fork () and exec () does not exist
        void child (pid_t parent)
        {
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT ("child (parent):% d\n"), parent));
        }
        int dumpRun (void)
        {
            ACE_TRACE ("Manager::dumpRun");

            if (ACE_OS::lseek (this->outputfd_, 0, SEEK_SET) == -1)
                ACE_ERROR_RETURN ((LM_ERROR, ACE_TEXT ("%p\n"),
                            ACE_TEXT ("lseek")), -1);

            char buf[1024];
            ssize_t length = 0;

            // Read the contents of the error stream written
            // by the child and print it out.
            while ((length = ACE_OS::read (this->outputfd_,
                            buf, sizeof(buf)-1)) > 0)
            {
                buf[length] = 0;
                ACE_DEBUG ((LM_DEBUG, ACE_TEXT ("%C\n"), buf));
            }

            ACE_OS::close (this->outputfd_);
            return 0;
        }

};

int createSfu(int sfu_id)
{
    int n_threads = 1;
    //number of threads to spawn
    //static int sfuId = 0;
    int retVal = 0;
    threadID = new ACE_thread_t[n_threads + 1];
    sfuThreadHandles =  new ACE_hthread_t[n_threads + 1];
    //sfuId++;
    if (ACE_Thread::spawn_n (threadID,	//id's for each of the threads
                n_threads,	//number of threads to spawn
                (ACE_THR_FUNC) sfuThread,	//entry point for new thread
                (void*)(intptr_t)sfu_id,	//args to worker
                THR_JOINABLE | THR_NEW_LWP,	//flags
                ACE_DEFAULT_THREAD_PRIORITY, 0, 0,
                sfuThreadHandles) == -1)
    {
        ACE_DEBUG ((LM_DEBUG, "Error in spawning thread\n"));
        retVal = -1;
    }
    else
    {
        retVal = 0;
    }
    return retVal;
}

ClientService::ClientService(const char *serverType):xGateProcessor(serverType)
{
  XGLOG_INFO( "ClientService::ClientService");
  pClntService = this;
}

ClientService::~ClientService(void)
{
  XGLOG_INFO( "ClientService::~ClientService");
}

xGateRetVal ClientService::process_msg(xGateBaseMsg* pMsg)
{
  XGLOG_INFO( "ClientService::process_msg(%s)", m_serverType);
  xGateMBCServiceMsg *ptmediaBCastMsg = dynamic_cast<xGateMBCServiceMsg*>(pMsg);

  if(decode_media_msg(ptmediaBCastMsg) != EN_XGATE_STATUS_SUCCESS) {
    XGLOG_ERROR( "MBCControllerProcessor::process_msg data mg_msg decode or validation failed");
  }

  if(ptmediaBCastMsg)
  {
    delete ptmediaBCastMsg;
    ptmediaBCastMsg = NULL;
  }

  return EN_XGATE_STATUS_SUCCESS;
}

xGateRetVal ClientService::decode_media_msg(xGateMBCServiceMsg *ptmediaBCastMsg)
{
  XGLOG_INFO( "Enter ClientService::decode_media_msg");

	if((xGateMSMsgType)ptmediaBCastMsg->getMsgType() == EN_XGATE_MSG_CONN_CLOSED){
		int sfuId = ptmediaBCastMsg->getSfuId();
		if(sendSfuDisconnectionToBridgeServer(ptmediaBCastMsg) != EN_XGATE_STATUS_SUCCESS){
  		XGLOG_INFO( "sendSfuDisconnectionToBridgeServer Failed");
			return EN_XGATE_STATUS_SUCCESS;
		}
		xGateMBCUtil::removeResIdFromSfuIdMap(sfuId);
		return EN_XGATE_STATUS_SUCCESS;
	}
  const char  *data = 0;
  unsigned int dataLen = 0;

  data = ptmediaBCastMsg->getMsg();
  dataLen = ptmediaBCastMsg->getMsgLen();  

  if(!data && dataLen <=0) {
    XGLOG_ERROR( "ClientService::decode_media_msg invalid data in xGateMgMsg.");
    return EN_XGATE_STATUS_ERROR;
  }
  int sfuConnId = ptmediaBCastMsg->getConnectionId();
  XGLOG_INFO( "ClientService::decode_media_msg data: %s, len: %d", data, dataLen);

  Document doc;
  MgMediaDetail clientMediaDetail;
  xGateMgMsgType mgMsgType;
  Value dataMemObj;

  xGateMBCServiceMsg *ptmediaBCastMsg1 = new xGateMBCServiceMsg();

  if(create_doc_object(doc, data, dataMemObj) != EN_XGATE_STATUS_SUCCESS) {
    XGLOG_INFO( "ClientService::decode_media_msg DOC object creation failed");
    return EN_XGATE_STATUS_ERROR;
  }

  mgMsgType = (xGateMgMsgType)doc["msg_type"].GetInt();

  switch(mgMsgType) 
  {
    case EN_XGATE_MG_SFU_ID:
    {
	Value::ConstMemberIterator itr = dataMemObj.MemberBegin();
	Value::ConstMemberIterator itrEnd = dataMemObj.MemberEnd();
	int sfuId(0);
	for(itr; itr != itrEnd; ++itr) {
	  if(!strcmp(itr->name.GetString(), "gateway_id")) {
	    sfuId = itr->value.GetUint();
	  }
	}
	XGLOG_INFO( "ClientService::decode_media_msg recvd EN_XGATE_MG_SFU_ID : %d",sfuId);
	int pbxConnId(0);
        int index = 0;
        MBCSfuMsg *mbc_sfu_msg;
	xGateMBCUtil::insert_sfuId_sfuConnId_map(sfuId,sfuConnId);
	xGateMBCUtil::get_pbxConnId_from_sfuId(sfuId,pbxConnId);
	xGateMBCUtil::insert_sfuConnId_pbxConnId_map(sfuConnId,pbxConnId);
  #if 0//changed to get recConnId from MBCConn map itself, no need to store for seperate sfuId
	int recConnId = -1;
  recConnId = xGateMBCUtil::get_recConnId_from_sfuId(sfuId);
  if(recConnId > -1)
	  xGateMBCUtil::insert_sfuConnId_recConnId_map(sfuConnId,recConnId);
  #endif
	delete ptmediaBCastMsg1;
	XGLOG_INFO( "m_mbcSfuMsgVect size: %d", xGateMBCUtil::m_mbcSfuMsgVect.size ());
        for (index = 0 ; index < xGateMBCUtil::m_mbcSfuMsgVect.size (); index++){
           mbc_sfu_msg = xGateMBCUtil::m_mbcSfuMsgVect.at (index);
           if (mbc_sfu_msg->sfuId == sfuId){
              xGateMBCServiceMsg *ptmediaBCastMsg1 = new xGateMBCServiceMsg();
	      ptmediaBCastMsg1->setSrcModuleId(IURDefines::UR_MODULE_XG_MBC_SERVICE);
	      ptmediaBCastMsg1->setDstModuleId(IURDefines::UR_MODULE_MEDIA_PROXY_SERVICE);
	      ptmediaBCastMsg1->setMsgType(EN_XGATE_MSG_XG_MBC_MODULE);
	      ptmediaBCastMsg1->set_mbc_event_type(EN_XG_MBC_EVENT_SEND);
	      XGLOG_INFO("Sending msg_type:7 sfuConnId: %d with len: %d",
                        sfuConnId, mbc_sfu_msg->len);
	      ptmediaBCastMsg1->setMsg(mbc_sfu_msg->msg);
	      ptmediaBCastMsg1->setMsgLen(mbc_sfu_msg->len);
	      ptmediaBCastMsg1->setConnectionId(sfuConnId);
	      XGMBCSERVICE->getConfig()->getCallBack()->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(ptmediaBCastMsg1));
              delete mbc_sfu_msg;
           }else {
              XGLOG_INFO ("mbc_sfu_msg->sfuId: %d != sfuId: %d ", mbc_sfu_msg->sfuId, sfuId);
           }
        }
        if (xGateMBCUtil::m_mbcSfuMsgVect.size()){
           xGateMBCUtil::m_mbcSfuMsgVect.clear();
        }
	delete []data;
	break;
      }
    case EN_XGATE_MG_RELEASE:
      {
	pid_t pid = 0;
	int sfuId(0);
	xGateMBCUtil::get_sfuId_from_sfuConnId(sfuConnId,sfuId);
	XGLOG_INFO( "ClientService::decode_media_msg handling EN_XGATE_MG_RELEASE from SFU id: %d ",sfuId);
	xGateMBCUtil::getPidFromMgSfuConnectionMap(sfuId, pid);
	//int const result = ACE_OS::kill (pid, SIGTERM);
	if(pid <= 0){
		XGLOG_ERROR("ClientService::EN_XGATE_MG_RELEASE failed SFU id:%d as pid is: %d",sfuId,pid);
		return EN_XGATE_STATUS_ERROR;
	}
	XGLOG_INFO( "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
	XGLOG_INFO( "$$$$$$$$$$$$$$$$$$$$$$$$ ClientService:: EN_XGATE_MG_RELEASE from SFU trying to kill pid:%ld $$$$$$$$",(long)pid);
	XGLOG_INFO( "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
	kill(pid,SIGKILL);
        xGateMBCUtil::removeSfuIdFromSfuConnIdMap(sfuId);
	xGateMBCUtil::removeSfuIdFromResIdMap(sfuId);
	xGateMBCUtil::removeResIdFromSfuIdMap(sfuId);
	char* msg = (char*)ptmediaBCastMsg->getMsg();
	delete []msg;
	delete ptmediaBCastMsg1;
	break;
      }
    case EN_XGATE_MG_MEDIA_DELETE:
    case EN_XGATE_MG_MEDIA_REQUEST_KEY_FRAME:
      {
	int sfuId(0);
	xGateMBCUtil::get_sfuId_from_sfuConnId(sfuConnId,sfuId);
	char* msg = (char*)ptmediaBCastMsg->getMsg();
	int len = ptmediaBCastMsg->getMsgLen();
	char* data = new char[len+4];
	memcpy(data,(char*)ptmediaBCastMsg->getMsg(),len-1);
	sprintf(&data[len-1],"\r\n\r\n");
	ptmediaBCastMsg1->setMsg(data);
	ptmediaBCastMsg1->setMsgLen(len+3);
	ptmediaBCastMsg1->setMsgType(EN_XGATE_MSG_XG_MBC_MODULE);
	ptmediaBCastMsg1->set_mbc_event_type(EN_XG_MBC_EVENT_SEND);
	XGLOG_INFO("Got msg from SFU with len %d",len);
	MBCTcpConnInfo mbcTcpConnInfo = ptmediaBCastMsg->get_mbc_tcp_conn_info();
	ptmediaBCastMsg1->set_mbc_tcp_conn_info(mbcTcpConnInfo);
	ptmediaBCastMsg1->setConnectionId(sfuConnId);
	ptmediaBCastMsg1->setSrcModuleId(IURDefines::UR_MODULE_XG_MBC_SERVICE);
	ptmediaBCastMsg1->setDstModuleId(IURDefines::UR_MODULE_MEDIA_PROXY_SERVICE);
	XGMBCSERVICE->getConfig()->getCallBack()->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(ptmediaBCastMsg1));
	delete []msg;
      }
      break;
    case EN_XGATE_MG_MEDIA_ADD_CLIENT_CONF_LIST:
    case EN_XGATE_MG_MEDIA_REQUEST_CREATE_SEND_CHANNEL:
      {
        int sfuId(0);
	xGateMBCUtil::get_sfuId_from_sfuConnId(sfuConnId,sfuId);
	XGLOG_INFO( "ClientService::decode_media_msg handling mgMsgType: %d from SFU id: %d ", mgMsgType, sfuId);
	char* msg = (char*)ptmediaBCastMsg->getMsg();
	int len = ptmediaBCastMsg->getMsgLen();
	char* data = new char[len+4];
	memcpy(data,(char*)ptmediaBCastMsg->getMsg(),len-1);
	sprintf(&data[len-1],"\r\n\r\n");
	ptmediaBCastMsg1->setMsg(data);
	ptmediaBCastMsg1->setMsgLen(len+3);
	ptmediaBCastMsg1->setMsgType(EN_XGATE_MSG_XG_MBC_MODULE);
	ptmediaBCastMsg1->set_mbc_event_type(EN_XG_MBC_EVENT_SEND);
	XGLOG_INFO("Got msg from SFU with len %d",len);
	MBCTcpConnInfo mbcTcpConnInfo = ptmediaBCastMsg->get_mbc_tcp_conn_info();
	ptmediaBCastMsg1->set_mbc_tcp_conn_info(mbcTcpConnInfo);
	ptmediaBCastMsg1->setConnectionId(sfuConnId);
	ptmediaBCastMsg1->setSrcModuleId(IURDefines::UR_MODULE_XG_MBC_SERVICE);
	ptmediaBCastMsg1->setDstModuleId(IURDefines::UR_MODULE_MEDIA_PROXY_SERVICE);
	XGMBCSERVICE->getConfig()->getCallBack()->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(ptmediaBCastMsg1));
	delete []msg;
        break;
      }
    case EN_XGATE_MG_VIDEO_REC_REQ:
      {
        int recConnId = -1;
        char* msg = (char*)ptmediaBCastMsg->getMsg();
        int len = ptmediaBCastMsg->getMsgLen();
        char* data = new char[len+4];
        memcpy(data,(char*)ptmediaBCastMsg->getMsg(),len-1);
        sprintf(&data[len-1],"\r\n\r\n");
        ptmediaBCastMsg1->setMsg(data);
        ptmediaBCastMsg1->setMsgLen(len+3);
        ptmediaBCastMsg1->setMsgType(EN_XGATE_MSG_XG_MBC_MODULE);
        ptmediaBCastMsg1->set_mbc_event_type(EN_XG_REC_EVENT_SEND);
        MBCTcpConnInfo mbcTcpConnInfo = ptmediaBCastMsg->get_mbc_tcp_conn_info();
        ptmediaBCastMsg1->set_mbc_tcp_conn_info(mbcTcpConnInfo);
        //xGateMBCUtil::get_recConnId_from_sfuConnId(ptmediaBCastMsg->getConnectionId(),recConnId);
        //Directly getting recConnId from MBCConn map itself
        recConnId = xGateMBCUtil::getRecConnIdFromMbcConnectionMap();
        if(recConnId == -1){
          XGLOG_INFO("Msg from SFU for Recording Server failed, RecConnId Not Found");
          break;
        }
        XGLOG_INFO("Got msg from SFU for Recording Server with len %d and recconnId:%d",len,recConnId);
        ptmediaBCastMsg1->setConnectionId(recConnId);
        ptmediaBCastMsg1->setSrcModuleId(IURDefines::UR_MODULE_XG_MBC_SERVICE);
        ptmediaBCastMsg1->setDstModuleId(IURDefines::UR_MODULE_MEDIA_PROXY_SERVICE);
        XGMBCSERVICE->getConfig()->getCallBack()->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(ptmediaBCastMsg1));
        delete []msg;
        break;
      }
    default:
      {
	int pbxConnId = 0;
	char* msg = (char*)ptmediaBCastMsg->getMsg();
	int len = ptmediaBCastMsg->getMsgLen();
	char* data = new char[len+4];
	memcpy(data,(char*)ptmediaBCastMsg->getMsg(),len-1);
	sprintf(&data[len-1],"\r\n\r\n");
	ptmediaBCastMsg1->setMsg(data);
	ptmediaBCastMsg1->setMsgLen(len+3);
	ptmediaBCastMsg1->setMsgType(EN_XGATE_MSG_XG_MBC_MODULE);
	ptmediaBCastMsg1->set_mbc_event_type(EN_XG_MBC_EVENT_SEND);
	XGLOG_INFO("Got msg from SFU with mgMsgType: %d len %d",mgMsgType, len);
	MBCTcpConnInfo mbcTcpConnInfo = ptmediaBCastMsg->get_mbc_tcp_conn_info();
	ptmediaBCastMsg1->set_mbc_tcp_conn_info(mbcTcpConnInfo);
	xGateMBCUtil::get_pbxConnId_from_sfuConnId(ptmediaBCastMsg->getConnectionId(),pbxConnId);
	ptmediaBCastMsg1->setConnectionId(pbxConnId);
	ptmediaBCastMsg1->setSrcModuleId(IURDefines::UR_MODULE_XG_MBC_SERVICE);
	ptmediaBCastMsg1->setDstModuleId(IURDefines::UR_MODULE_MEDIA_PROXY_SERVICE);
	XGMBCSERVICE->getConfig()->getCallBack()->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(ptmediaBCastMsg1));
	delete []msg;
	break;
      }
  }
  return EN_XGATE_STATUS_SUCCESS;
}

xGateRetVal ClientService::create_doc_object(Document &doc, const char *jsonData, Value& dataMemObj)
{
     doc.Parse(jsonData);
    if (doc.HasParseError()) {
        XGLOG_ERROR( "ClientService::create_doc_object json parse error");
        return EN_XGATE_STATUS_ERROR;
    } else if(!doc.IsObject()) {
        XGLOG_ERROR( "ClientService::create_doc_object not able to created document object");
        return EN_XGATE_STATUS_ERROR;
    } else if(!doc.HasMember("msg_type")) {
        XGLOG_ERROR( "ClientService::create_doc_object msg_type field is missing in json msg");
        return EN_XGATE_STATUS_ERROR;
    } else if(!doc.HasMember("data")) {
        XGLOG_ERROR( "xGateSrvrProcessor::create_doc_object data member not found in json msg");
        return EN_XGATE_STATUS_ERROR;
    }
    dataMemObj = doc["data"];
    return EN_XGATE_STATUS_SUCCESS;
}

xGateRetVal ClientService::sendSfuDisconnectionToBridgeServer(xGateMBCServiceMsg *ptmediaBCastMsg)
{
	int len,SfuId = ptmediaBCastMsg->getSfuId();
	string Uid(""), Json("");
	if(!(xGateMBCUtil::getResIdFromSfuIdMap(SfuId, Uid))){
		XGLOG_ERROR( "ClientService::sendSfuDisconnectionToBridgeServer Failed Uid Not Found !!");
	  return EN_XGATE_STATUS_ERROR;
	}
  
  MgMediaDetail clientMediaDetail;
  xGateMgMsgType mgMsgType = EN_XGATE_MG_SFU_CONN_CLOSED;

	clientMediaDetail.mgresource_id = Uid;
	clientMediaDetail.call_id = Uid;
	clientMediaDetail.call_type = EN_XGATE_CALL_TYPE_CONFERENCE_VIDEO; 
  
	len = xGateMBCUtil::create_media_msg(clientMediaDetail, Json, mgMsgType);

	xGateMBCServiceMsg *ptmediaBCastMsg1 = new xGateMBCServiceMsg();
	int pbxConnId = 0;
	char* msg = (char*)Json.c_str();
	//int len = ptmediaBCastMsg->getMsgLen();
	char* data = new char[len];
	memcpy(data,(char*)Json.c_str(),len);
	//sprintf(&data[len-1],"\r\n\r\n");
	ptmediaBCastMsg1->setMsg(data);
	ptmediaBCastMsg1->setMsgLen(len);
	ptmediaBCastMsg1->setMsgType(EN_XGATE_MSG_XG_MBC_MODULE);
	ptmediaBCastMsg1->set_mbc_event_type(EN_XG_MBC_EVENT_SEND);
	XGLOG_INFO("sendSfuDisconnectionToBridgeServer with length %d",len);
	MBCTcpConnInfo mbcTcpConnInfo = ptmediaBCastMsg->get_mbc_tcp_conn_info();
	ptmediaBCastMsg1->set_mbc_tcp_conn_info(mbcTcpConnInfo);
	xGateMBCUtil::get_pbxConnId_from_sfuConnId(ptmediaBCastMsg->getConnectionId(),pbxConnId);
	ptmediaBCastMsg1->setConnectionId(pbxConnId);
	ptmediaBCastMsg1->setSrcModuleId(IURDefines::UR_MODULE_XG_MBC_SERVICE);
	ptmediaBCastMsg1->setDstModuleId(IURDefines::UR_MODULE_MEDIA_PROXY_SERVICE);
	XGMBCSERVICE->getConfig()->getCallBack()->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(ptmediaBCastMsg1));
	//delete []msg;
	return EN_XGATE_STATUS_SUCCESS;
}

void *sfuThread (void *arg)
{
    int *sfu_id = NULL;
    sfu_id = (int*)&arg;
		int sfuid = *sfu_id;
    Manager m ("GET_PROCESS");
    m.doWork (sfuid);
    return 0;
}

