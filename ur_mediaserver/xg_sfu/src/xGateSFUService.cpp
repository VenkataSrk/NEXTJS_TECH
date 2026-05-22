#include <fstream>
#include <stdexcept>
#include "xGateUtil.h"
#include "xglog.h"
#include "xGateBaseMsg.h"
#include "xGateSFUService.h"
#include "xGateHttpHandler.h"
#include "xGateSFUtil.h"
#include "SFUBridgeProcessor.h"
#include "SFUController.h"
#include "xGateSFUServiceMsg.h"
#include "IURModule.h"
#include "XGConfig.h"
xGateSFUService::xGateSFUService() :
  ACE_Task<ACE_MT_SYNCH>(),
  m_run(false),m_sfuConfig(0)
{
   m_strModuleName ="SFUModule";
}

xGateSFUService::~xGateSFUService(void)
{
}

int xGateSFUService::initModule(IURModuleConfig * psfu_config)
{
   m_sfuConfig = psfu_config;
   XGConfig *pConfig = dynamic_cast<XGConfig*>(psfu_config);
   //init(pConfig->m_sfu_id);
   init();
   return IURDefines::UR_RETURN_SUCCESS;
}
int xGateSFUService::unitModule()
{
   stop();
   return IURDefines::UR_RETURN_SUCCESS;
}
void xGateSFUService::getModuleName(std::string& moduleName){
    moduleName=m_strModuleName;
}

IURDefines::MODULE_ID xGateSFUService::getModuleID(){
  return IURDefines::UR_MODULE_SFU_SERVICE;
}

IURModuleConfig * xGateSFUService::getModuleConfig(){
  return m_sfuConfig;
}

bool xGateSFUService::stop(void)
{
  m_run = false;
  msg_queue()->deactivate();
  wait();
  XGLOG_INFO("xGateSFUService stopped !");
  return true;
}

//void xGateSFUService::init_sfu(int sfu_id)
void xGateSFUService::init_sfu()
{
  // SFU Thread initialization
  SFUBridgeProcessor *psfuBridgeProcessor = new SFUBridgeProcessor("SFUBridgeProcess");
  if(psfuBridgeProcessor->init(1) != EN_XGATE_STATUS_SUCCESS)
  {
    cout << "SFUBridgeProcessor initialized" << endl;
    return EN_XGATE_STATUS_ERROR;
  }
  xGateSFUtil::setSFUBridgeProcessor(psfuBridgeProcessor);

  SFUController *pSfuController = new SFUController("SFUController");
  if(pSfuController->init() != EN_XGATE_STATUS_SUCCESS)
  {
      cout << "pSfuController initialized from main thread" << endl;
      return EN_XGATE_STATUS_ERROR;
  }
  xGateSFUtil::setSFUController(pSfuController);
  
   SFUClient *psfuClient  = new SFUClient();
   if(!psfuClient){
      XGLOG_ERROR("Could not make new SFU Client");
      return EN_XGATE_STATUS_ERROR;      
   }
   //psfuClient->set_sfu_id(sfu_id);
   //xGateSFUtil::setSFUId(sfu_id);
   xGateSFUtil::setSFUClient(psfuClient); 
}

/*
bool xGateSFUService::make_connection()
{
 XGLOG_INFO("Making connection with MBC");
 string m_ip;
 unsigned int m_port;


 vector<TcpLisentAddr> &peerConnectAddrs = CONFIGSERVICE->m_mediaModuleProfile.m_peerConnectAddrs;
 vector<TcpLisentAddr>::iterator itrConnAddr = peerConnectAddrs.begin();
  
  m_ip = (*itrConnAddr).m_ip;
  m_port = (*itrConnAddr).m_port;

 cout<<"The mg address "<<m_ip<<endl;
 cout<<"The mg port"<<m_port<<endl;


 xGateSFUServiceMsg *pmsg = new xGateSFUServiceMsg();
 pmsg->setDstModuleId(IURDefines::UR_MODULE_MEDIA_PROXY_SERVICE); 
 pmsg->setSrcModuleId(IURDefines::UR_MODULE_SFU_SERVICE); 
 pmsg->setMsgType((xGateMsgType)EN_XGATE_MSG_SFU_CONNECT_CLIENT);
 //pmsg->set_conn_ip(CONFIGSERVICE->m_localIp4Addr);

 pmsg->set_conn_ip(m_ip);
 //pmsg->set_port_number(SFU_LISTEN_PORT);
 pmsg->set_port_number(m_port);

 SFUSERVICE->getConfig()->getCallBack()->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(pmsg));
}
*/

//bool xGateSFUService::init(int sfu_id)
bool xGateSFUService::init()
{
  XGLOG_INFO("xGateSFUService::init() called");
  m_run = true;



  // Start the http thread.
  if (activate(THR_NEW_LWP, 1) == -1 )
  {
    XGLOG_ERROR("xGateSFUService::init() failed to start SFUService thread !");
    return false;
  }
  else
  {
    XGLOG_INFO("xGateSFUService::init() success !");
  }

  //init_sfu(sfu_id);
  init_sfu();

  //make_connection();

  initTcpClientListenRequest();

  return true;
}

int xGateSFUService::svc(void)
{
  //CLog::Warning(THISMODULE, "xGateSFUService::svc(%s)", m_serverType);
  ACE_Message_Block *pAmb = 0;

  while(m_run)
  {
    int result = getq(pAmb, NULL);
    if ( ( result == -1 ) && ( errno == EWOULDBLOCK ) )
    {
      continue;
    }
    if ((result != -1))
    {
      handle_msg(pAmb);
    }
    else
    {
      XGLOG_ERROR("xGateSFUService::svc Error in Message Queue");
      m_run = false;
    }
    // delete the message received
    if(pAmb)
    {
      delete pAmb;
      pAmb = NULL;
    }
  }

  XGLOG_INFO("xGateSFUService::svc() stopped");
  return 0;
}

bool xGateSFUService::handle_msg(ACE_Message_Block *pAmb)
{
  if(pAmb != NULL) {
    xGateBaseMsg* pMsg = NULL;
    XGLOG_ERROR("xGateSFUService::handle_msg recvd");
    pMsg = dynamic_cast<xGateBaseMsg*>(pAmb);
    if(pMsg  == NULL) {
      XGLOG_ERROR("xGateSFUService::handle_msg recvd invalid xGateBaseMsg Message");
      return false;
    }
    //send http request
    xGateSFUServiceMsg *pSfuMsg = dynamic_cast<xGateSFUServiceMsg*>(pAmb);
    switch((xGateSFUMsgType)pSfuMsg->getMsgType())
    {
     case EN_XGATE_MSG_SFU_CLIENT_CONNECTED:
     {
       XGLOG_INFO("xGateSFUService::handle_msg recvd msg EN_XGATE_MSG_CLIENT_CONNECTED");
       SFUClient *psfuClient = xGateSFUtil::getSFUClient();
       unsigned int connection_id = pSfuMsg->get_connection_id();
       psfuClient->set_conn_id(connection_id);       
       break;	     
     }	     
     case EN_XGATE_MSG_MBC_SERVER:
     {
       SFUClient *psfuClient = xGateSFUtil::getSFUClient();
       unsigned int connection_id = pSfuMsg->get_connection_id();
       psfuClient->set_conn_id(connection_id);       
       char *data = pSfuMsg->getMsg();
       psfuClient->get_mbc_msg(data,pSfuMsg->getMsgLen());
       if (data){
         delete[] data;
         pSfuMsg->setMsgLen(0);
         pSfuMsg->setMsg(NULL);
       }
       break;	     
     }
     case EN_XGATE_MSG_SFU_CONN_CLOSED:
     {
      XGLOG_INFO("xGateSFUService::handle_msg recvd msg EN_XGATE_MSG_CONN_CLOSED for pid:%d",getpid());
      //exit(0);       
      break;
     }	     
     default:
     {
       XGLOG_ERROR("xGateSFUService::handle_msg recvd no matching case");
       break;
     }       
    }
  return true;
 }
}  



int xGateSFUService::pushModuleMsg(IURModuleMsg * psfuMsg)
{
     XGLOG_TRACE("xGateSFUService::inside pushModuleMsg");   
     if (!psfuMsg) {
          XGLOG_ERROR("pushModuleMsg::Message not received from controller.!");
          return IURDefines::UR_RETURN_ERROR;
       }
      else{
         XGLOG_TRACE("xGateSFUService::Message received from controller to SFU");
          xGateSFUService *sfuService = SFUSERVICE;
           sfuService->putq(dynamic_cast<ACE_Message_Block*>(psfuMsg));
      }    
  return IURDefines::UR_RETURN_SUCCESS;
}


bool xGateSFUService::initTcpClientListenRequest()
{
  XGLOG_INFO("Initiating  RecordingSFU listen request for MBC communication");


  xGateSFUServiceMsg *pmsg = new xGateSFUServiceMsg();
  pmsg->setSrcModuleId(IURDefines::UR_MODULE_SFU_SERVICE);
  pmsg->setDstModuleId(IURDefines::UR_MODULE_MEDIA_PROXY_SERVICE);
  pmsg->setMsgType((xGateMsgType)EN_XGATE_MSG_REC_SERVER);
  pmsg->set_conn_ip(CONFIGSERVICE->m_localIp4Addr);
  pmsg->set_port_number(REC_LISTEN_PORT);
  SFUSERVICE->getConfig()->getCallBack()->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(pmsg));

  return true;
}




