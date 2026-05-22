//local includes
#include "xglog.h"
#include "xGateSchedulerHandler.h"
#include "xGateSchedulerService.h"
#include "xGateSchedulerServiceMsg.h"
#include "xGateSchedulerMsg.h"

xGateSchedulerHandler* xGateSchedulerHandler::m_instance = NULL;

xGateSchedulerHandler::xGateSchedulerHandler()
{}

xGateSchedulerHandler::~xGateSchedulerHandler()
{}

xGateSchedulerHandler::xGateSchedulerHandler(const xGateSchedulerHandler& rhs)
{}

xGateSchedulerHandler& xGateSchedulerHandler::operator=(const xGateSchedulerHandler& rhs)
{}

xGateSchedulerHandler * xGateSchedulerHandler::getInstance(void)
{
    if(m_instance == NULL) {
      m_instance = new xGateSchedulerHandler();
      if(!m_instance) {
        XGLOG_ERROR("HttpHandler scheduler-handler initialization failed !");
        delete m_instance;
        m_instance = NULL;
      } else {
        XGLOG_INFO("HttpHandler scheduler-handler initialized successfully !");
      }
  }
  return m_instance;
}

void xGateSchedulerHandler::deleteInstance(void)
{
    if(m_instance != NULL) {
      delete m_instance;
      m_instance = NULL;
      XGLOG_INFO("HttpHandler scheduler-handler de-initialized successfully !");
    }
}

bool xGateSchedulerHandler::doSchedulerRequest(SchedulerInfo &schedulerInfo)
{
  XGLOG_INFO("xGateSchedulerHandler::doSchedulerRequest called");
  int timerFunc = schedulerInfo.m_enumTimerFunc;
  xGateSchedulerHandler *schedulerHandler = xGateSchedulerHandler::getInstance();
  ACE_Reactor *reactor = schedulerHandler->getReactor();
  string uid = schedulerInfo.m_contextId;
  SchedulerType schedulerType = schedulerInfo.m_schedulerType;
  int retryCount = schedulerInfo.m_retryCount;
  unsigned int timeoutSecond = schedulerInfo.m_timeoutSecond;
  IURDefines::MODULE_ID module = schedulerInfo.m_requestorModule;
  void* ptCtxData = schedulerInfo.m_pCtxdata;
  long int timerId = -1;

  switch(timerFunc)
  {
    case SCHEDULER_FUNCTION_SET_TIMER:
    {
		  if(m_tSchedulerContext.getSchedulerContext(uid, schedulerType))
			{
        XGLOG_ERROR("xGateSchedulerHandler::doSchedulerRequest failed. SchedulerContext already exists for uid: %s and schedulerType:(%d) !", \
				            uid.c_str(), schedulerType);
				return false;
			}
      xGateTimerData *data = new xGateTimerData(uid, schedulerType, retryCount, module, ptCtxData);
      timerId = reactor->schedule_timer(schedulerHandler, (void*)data, ACE_Time_Value(timeoutSecond, 0));
      if(!m_tSchedulerContext.insertSchedulerContext(uid, schedulerType, timerId))
      {
        XGLOG_ERROR("SCHEDULER_FUNCTION_SET_TIMER insertSchedulerContext failed - %s", uid.c_str());
      } else {
        XGLOG_INFO("SCHEDULER_FUNCTION_SET_TIMER insertSchedulerContext success - %s", uid.c_str());
      }
      if(timerId == -1) 
      {
        XGLOG_ERROR("xGateSchedulerHandler::doSchedulerRequest failed for uid: %s !", \
            uid.c_str());
      } else {
        XGLOG_INFO("xGateSchedulerHandler::doSchedulerRequest for uid: %s success with timer_id: %ld", \
            uid.c_str(), timerId);
      }
      break;
    }
    case SCHEDULER_FUNCTION_CANCEL_TIMER:
    {
      timerId = m_tSchedulerContext.getSchedulerContext(uid, schedulerType);
      if(reactor->cancel_timer(timerId)) 
      {
        XGLOG_DEBUG("xGateSchedulerHandler::cancel_timer success for uid: %s and timer_id: %ld", \
            uid.c_str(), timerId);
        if(!m_tSchedulerContext.deleteSchedulerContext(uid, schedulerType))
        {
          XGLOG_ERROR("SCHEDULER_FUNCTION_CANCEL_TIMER deleteSchedulerContext failed - %s", uid.c_str());
        } else {
          XGLOG_INFO("SCHEDULER_FUNCTION_CANCEL_TIMER deleteSchedulerContext success - %s", uid.c_str());
        } 
      } else {
        XGLOG_ERROR("cancel_timer failed for timer_id: %ld and uid: %s", \
            timerId, uid.c_str());
      }
      break;
    }
   }
  return true;
}

int xGateSchedulerHandler::handle_timeout(const ACE_Time_Value&, const void *arg)
{
  XGLOG_DEBUG("handle_timeout triggered");
  xGateTimerData *data = (xGateTimerData*)arg;
  if(!data) {
    XGLOG_ERROR("handle_timeout failed. invalid xGateTimerData !");
    return 0;
  }

  string uid = data->m_uid.c_str();
  if(uid.empty()) {
    XGLOG_ERROR("handle_timeout failed. uid is empty !");
    return 0;
  }

  switch(data->m_timerType) {
    case EN_SCHEDULER_URCONF_USER_JOIN_WAIT:
      {
        XGLOG_DEBUG("EN_SCHEDULER_URCONF_USER_JOIN_WAIT timeout occurred for uid: %s", uid.c_str());
        if(!m_tSchedulerContext.deleteSchedulerContext(uid, EN_SCHEDULER_URCONF_USER_JOIN_WAIT))
        {
          XGLOG_ERROR("handle_timeout deleteSchedulerContext failed - %s", uid.c_str());
        } else {
          XGLOG_INFO("handle_timeout deleteSchedulerContext success - %s", uid.c_str());
        }
        break;
      }
    case EN_SCHEDULER_URCONF_PORT_ALLOC_RES:
      {
        XGLOG_DEBUG("EN_SCHEDULER_URCONF_PORT_ALLOC_RES timeout occurred for uid: %s", uid.c_str());
        if(!m_tSchedulerContext.deleteSchedulerContext(uid, EN_SCHEDULER_URCONF_PORT_ALLOC_RES))
        {
          XGLOG_ERROR("handle_timeout deleteSchedulerContext failed - %s", uid.c_str());
        } else {
          XGLOG_INFO("handle_timeout deleteSchedulerContext success - %s", uid.c_str());
        }
        break;
      }
    case EN_SCHEDULER_URCONF_ACTIVE_SPEAKER_INFO:
      {
        XGLOG_DEBUG("EN_SCHEDULER_URCONF_ACTIVE_SPEAKER_INFO timeout occurred for uid: %s", uid.c_str());
        if(!m_tSchedulerContext.deleteSchedulerContext(uid, EN_SCHEDULER_URCONF_ACTIVE_SPEAKER_INFO))
        {
          XGLOG_ERROR("handle_timeout deleteSchedulerContext failed - %s", uid.c_str());
        } else {
          XGLOG_INFO("handle_timeout deleteSchedulerContext success - %s", uid.c_str());
        }
        break;
      }
    case EN_SCHEDULER_URCONF_USER_DISCONNECT_NOTIFY:
      {
        XGLOG_DEBUG("EN_SCHEDULER_URCONF_USER_DISCONNECT_NOTIFY timeout occurred for uid: %s", uid.c_str());
        if(!m_tSchedulerContext.deleteSchedulerContext(uid, EN_SCHEDULER_URCONF_USER_DISCONNECT_NOTIFY))
        {
          XGLOG_ERROR("handle_timeout deleteSchedulerContext failed - %s", uid.c_str());
        } else {
          XGLOG_INFO("handle_timeout deleteSchedulerContext success - %s", uid.c_str());
        }
        break;
      }
    case EN_SCHEDULER_URCONF_ACD_RES:
      {
        XGLOG_DEBUG("EN_SCHEDULER_URCONF_ACD_RES timeout occurred for uid: %s", uid.c_str());
        if(!m_tSchedulerContext.deleteSchedulerContext(uid, EN_SCHEDULER_URCONF_ACD_RES))
        {
          XGLOG_ERROR("handle_timeout deleteSchedulerContext failed - %s", uid.c_str());
        } else {
          XGLOG_INFO("handle_timeout deleteSchedulerContext success - %s", uid.c_str());
        }
        break;
      }
    default:
      {
        XGLOG_DEBUG("Unknown/Unsupported timeout event triggered for uid: %s", uid.c_str());
        return 0;
      }
  }

  xGateSchedulerMsg *pMsg = new xGateSchedulerMsg();
  pMsg->setDstModuleId(data->m_requestorModule);
  pMsg->setUid(uid);
  pMsg->setTimerData(data);
  pMsg->setMsgType(EN_XGATE_MSG_TIMER);
  XGUTIL->post(pMsg);
  
  return 0;
}
