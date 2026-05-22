#include "BSCallObject.h"
#include "LogHandler.h"

/*-------------------------------------------------------BSCallObject  Implementation------------------------------------------------------------------------------------------------------*/

BSCallObject::BSCallObject(std::string callID,std::string caIP, unsigned short caPort){
    logdebug(callID,"BSCallObject::BSCallObject() IP %s Port %d",caIP.c_str(), caPort );
    setUid(callID);
    setCallAgentIP(caIP);
    setCAllAgentPort(caPort);
    setCallState(BSCallState::STATE_INIT);
    m_pBusinessOperations =  new BSOperation(callID);
}
BSCallObject::~BSCallObject(){
   if(m_pBusinessOperations) {
    delete m_pBusinessOperations;
    m_pBusinessOperations = nullptr;
   }

}
CA::CAMessageBlock* BSCallObject::handleConfigUpdateEvent(BSCallConfObject*bsCallConf ){

    if(!bsCallConf)
        return nullptr;
    
    CA::CAMessageBlock* pMsg = nullptr;
    switch(getCallState()){
        case BSCallState::STATE_INIT:
        setCallState(BSCallState::STATE_PROCESSING);
        //m_pBusinessOperations->updateConfig(bsCallConf) ;//updateConfig(bsCallConf);
        //pMsg = m_pBusinessOperations->spawnTask();
        break;
        default:
        break;
    }
    return pMsg;
}



BSOperationMessage* BSCallObject::callSetupInd(BSParam* bsp){
    if(!bsp){
        setLastKnownError("ERROR - callSetupInd BSParam is NULL");
        return nullptr;
    }
    if(getCallState() == BSCallState::STATE_INIT){
        if(!m_pBusinessOperations){
            m_pBusinessOperations = new BSOperation(getUid());
            if(!m_pBusinessOperations) return nullptr;
        }
        bsp->setState(BSCallState::STATE_INIT);
        std::unique_ptr<BSParam> pbsParam(bsp); 
        m_pBusinessOperations->updateInParam(std::move(pbsParam));
        BSOperationMessage* pMsg =  new BSOperationMessage();
        if(pMsg){
            std::unique_ptr<BSOperation> bsopr(new BSOperation(getUid()));                
            if(bsopr){
                bsopr->copyOperation(m_pBusinessOperations);
                pMsg->m_bsOperation =  std::move(bsopr);
                pMsg->setMsgType(MESSAGEEVENT::MSG_BSP2BSOP);
                pMsg->setEvent((int)OPERATIONEVENT::OPE_TASK_INIT);
                setCallState(BSCallState::STATE_PROCESSING);
                setLastKnownError("SUCCESS - Call state set to Processing in callSetupInd()");
                return pMsg;
            }else{setLastKnownError("Operation creation error");}
        }else{setLastKnownError("Operation Message creation error ");}
    }else{setLastKnownError("Error in Call State -- Not in Initate state");}
    return nullptr;
}
BSOperationMessage* BSCallObject::callProceedingInd(BSParam* bsp){
    if(getCallState() == BSCallState::STATE_DIALING) {
     setCallState(BSCallState::STATE_PROCEEDING);
     setLastKnownError("BSCallObject::callProceedingInd() update call state to STATE_PROCEEDING");
    }
    if(getCallState() == BSCallState::STATE_PROCEEDING) {
     std::unique_ptr<BSParam> pbsParam(bsp);
     m_pBusinessOperations->updateInParam(std::move(pbsParam));
    }
    return nullptr;
}
BSOperationMessage* BSCallObject::callAlertingInd(BSParam* bsp){
    if(getCallState() >= BSCallState::STATE_DIALING) {
        setCallState(BSCallState::STATE_ALERTING);
        setLastKnownError("BSCallObject::callAlertingInd() update call state to STATE_ALERTING");
    }
    if(getCallState() == BSCallState::STATE_ALERTING) {
     std::unique_ptr<BSParam> pbsParam(bsp);
     m_pBusinessOperations->updateInParam(std::move(pbsParam));
    }
    return nullptr;
}
BSOperationMessage* BSCallObject::callConnectedInd(BSParam* bsp){
    
    if(getCallState()  >= BSCallState::STATE_DIALING && getCallState()  < BSCallState::STATE_CONNECTED ){
        setCallState(BSCallState::STATE_CONNECTED);
        setLastKnownError("BSCallObject::callConnectedInd() update call state to STATE_CONNECTED");
        if(!m_pBusinessOperations){
            m_pBusinessOperations = new BSOperation(getUid());
            if(!m_pBusinessOperations) return nullptr;
        }
        std::unique_ptr<BSParam> pbsParam(bsp);
        m_pBusinessOperations->updateInParam(std::move(pbsParam));
        m_pBusinessOperations->setTaskId(0);
        m_pBusinessOperations->getInParam()->setState(BSCallState::STATE_CONNECTED);
        m_pBusinessOperations->getInParam()->setconnectionState(1);

        m_pBusinessOperations->setCallConnectedStatus(1);
        m_pBusinessOperations->setCallTime(BSCallState::STATE_CONNECTED);
        
        BSOperationMessage* pMsg =  new BSOperationMessage();
        if(pMsg){ 
            std::unique_ptr<BSOperation> bsopr(new BSOperation(getUid()));                
            if(bsopr){
                bsopr->copyOperation(m_pBusinessOperations);
                pMsg->setMsgType(MESSAGEEVENT::MSG_BSP2BSOP);
                pMsg->setEvent((int)OPERATIONEVENT::OPE_TASK_CONNECTED);
                pMsg->m_bsOperation =  std::move(bsopr);
                return pMsg;
            }
        }
    }
    return nullptr;
}

BSOperationMessage* BSCallObject::callRouteErrorInd(BSParam* bsp)
{
	if(getCallState()  >= BSCallState::STATE_DIALING && getCallState()  < BSCallState::STATE_CONNECTED ){
		//setCallState(BSCallState::STATE_ROUTE_ERROR);
		setLastKnownError("BSCallObject::callRouteErrorInd() update call Route error");
		if(!m_pBusinessOperations){
			m_pBusinessOperations = new BSOperation(getUid());
			if(!m_pBusinessOperations) return nullptr;
		}
                std::unique_ptr<BSParam> pbsParam(bsp);
                m_pBusinessOperations->updateInParam(std::move(pbsParam));
                setCallState(BSCallState::STATE_PROCESSING); //changing Dialing state to processing state
		m_pBusinessOperations->setTaskId(0);
		m_pBusinessOperations->getInParam()->setState(BSCallState::STATE_ROUTE_ERROR);
		m_pBusinessOperations->getInParam()->setconnectionState(0);
		m_pBusinessOperations->setCallConnectedStatus(0);
		m_pBusinessOperations->setCallTime(BSCallState::STATE_ROUTE_ERROR);

		BSOperationMessage* pMsg =  new BSOperationMessage();
		if(pMsg){
			std::unique_ptr<BSOperation> bsopr(new BSOperation(getUid()));
			if(bsopr){
				bsopr->copyOperation(m_pBusinessOperations);
				pMsg->setMsgType(MESSAGEEVENT::MSG_BSP2BSOP);
				pMsg->setEvent((int)OPERATIONEVENT::OPE_TASK_ROUTE_ERROR);
				pMsg->m_bsOperation =  std::move(bsopr);
				return pMsg;
			}
		}
	}
	return nullptr;
}
BSOperationMessage* BSCallObject::callDisconnectedInd(BSParam* bsp){
   
    if(!m_pBusinessOperations){
        m_pBusinessOperations = new BSOperation(getUid());
        if(!m_pBusinessOperations) return nullptr;
        
    }
    std::unique_ptr<BSParam> pbsParam(bsp); 
    m_pBusinessOperations->updateInParam(std::move(pbsParam)); 
    if(getCallState() <  BSCallState::STATE_CONNECTED){
        setCallState(BSCallState::STATE_DISCONNECTED);
         setLastKnownError("BSCallObject::callDisconnectedInd() update call state to STATE_DISCONNECTED on preConnected");     
            m_pBusinessOperations->setCallTime(BSCallState::STATE_CONNECTED);
            m_pBusinessOperations->setCallTime(BSCallState::STATE_DISCONNECTED);
            m_pBusinessOperations->getInParam()->setState(BSCallState::STATE_DISCONNECTED);
            m_pBusinessOperations->setCallConnectedStatus(0);
            m_pBusinessOperations->setTaskId(0);
    }else if(getCallState() <  BSCallState::STATE_DISCONNECTED &&  getCallState() >= BSCallState::STATE_CONNECTED){
        setCallState(BSCallState::STATE_DISCONNECTED);  
            setLastKnownError("BSCallObject::callDisconnectedInd() update call state to STATE_DISCONNECTED on postConnected");     
            m_pBusinessOperations->setCallTime(BSCallState::STATE_DISCONNECTED);
            m_pBusinessOperations->getInParam()->setState(BSCallState::STATE_DISCONNECTED);
            m_pBusinessOperations->setTaskId(0);
    }
    
    m_pBusinessOperations->setTaskId(0);
    std::unique_ptr<BSOperation> bsopr(new BSOperation(getUid())); 
    if(bsopr){
        bsopr->copyOperation(m_pBusinessOperations);
        BSOperationMessage* pMsg =  new BSOperationMessage();
        if(pMsg){
            pMsg->setMsgType(MESSAGEEVENT::MSG_BSP2BSOP);
            pMsg->setEvent((int)OPERATIONEVENT::OPE_TASK_DEINIT);
            pMsg->m_bsOperation =  std::move(bsopr);
            return pMsg;
        }
    }
    return nullptr;
}
BSOperationMessage* BSCallObject::callClearInd(){
    setCallState(BSCallState::STATE_CLEAR);
    /*BSOperationMessage* pMsg =  new BSOperationMessage();
    if(pMsg){
        pMsg->setMsgType(MESSAGEEVENT::MSG_BSP2BSOP);
        pMsg->setEvent((int)OPERATIONEVENT::);
        pMsg->m_bsOperation =  std::move(bsopr);
        return pMsg;
    }*/
    #if 1
        std::unique_ptr<BSOperation> bsOpr(new BSOperation(getUid()));
        if(bsOpr){
            bsOpr->copyOperation(m_pBusinessOperations);
            BSOperationMessage* pMsg =  new BSOperationMessage;
            if(pMsg){
                pMsg->m_bsOperation =  std::move(bsOpr);
                pMsg->setMsgType(MESSAGEEVENT::MSG_BSP2BSOP);
                pMsg->setEvent((int)OPERATIONEVENT::OPE_TASK_DEINIT);
                setLastKnownError("BSCallObject::callTimeOutInd OPERATIONEVENT::OPE_TASK_DEINIT Rquested");
                return pMsg;
            }else{setLastKnownError("Business Message crearion error callTimeOutInd");}
        }else{setLastKnownError("BSCallObject::callClearInd() update call state to STATE_CLEAR");}
#endif
        return nullptr;
}
void BSCallObject::callDialingInd(BSParam* bsp){
    if(getCallState() == BSCallState::STATE_PROCESSING ){
        setCallState(BSCallState::STATE_DIALING);
        if(bsp){
            std::unique_ptr<BSParam> bsparam (bsp)  ;
            m_pBusinessOperations->updateInParam(std::move(bsparam));
        }

    }
    setLastKnownError("BSCallObject::callDialingInd() update call state to STATE_DIALING");
}
BSOperationMessage* BSCallObject::callHttpResponseInd(BSOperation* bsp){
    if(!bsp){
        setLastKnownError("Operation Object is null");
        return nullptr;
    }
    
    m_pBusinessOperations->copyOperation(bsp);
    std::unique_ptr<BSOperation> bsOpr( bsp);
    if(bsOpr){
        BSOperationMessage* pMsg = new BSOperationMessage();
        pMsg->m_bsOperation =  std::move(bsOpr);
        pMsg->setMsgType(MESSAGEEVENT::MSG_BSP2BSOP);
        pMsg->setEvent((int)OPERATIONEVENT::OPE_HTTP_RESPONSE);
        return pMsg;
    }
    return nullptr;
}
BSOperationMessage* BSCallObject::callChargingcycleInd(int taskId){
    setLastKnownError("BSCallObject::CallChargingCycleInd Request completed");
    if(getCallState() == BSCallState::STATE_CONNECTED){
        m_pBusinessOperations->setTaskId(taskId);
        std::unique_ptr<BSOperation> bsOpr(new BSOperation(getUid()));
        if(bsOpr){
            bsOpr->copyOperation(m_pBusinessOperations);
            BSOperationMessage* bsOprMsg = new BSOperationMessage;
            if(bsOprMsg){
                bsOprMsg->m_bsOperation =  std::move(bsOpr);;
                bsOprMsg->setMsgType(MESSAGEEVENT::MSG_BSP2BSOP);
                bsOprMsg->setEvent((int)OPERATIONEVENT::OPE_CHARING_CYCLE);
                return bsOprMsg;
            }else{setLastKnownError("Business Message crearion error CallChargingCycleInd");}
        }else{setLastKnownError("Business Operation creatin error CallChargingCycleInd");}
    }else{setLastKnownError("Call not in Connected State CallChargingCycleInd");}
    return nullptr;
}
BSOperationMessage* BSCallObject::callTimeOutInd(){
    setLastKnownError("BSCallObject::callTimeOutInd");
    if(getCallState() < BSCallState::STATE_CONNECTED){
        m_pBusinessOperations->setSipErrorCode(408);
        m_pBusinessOperations->getInParam()->setErrorCode(408);
        std::unique_ptr<BSOperation> bsOpr(new BSOperation(getUid()));
        if(bsOpr){
            bsOpr->copyOperation(m_pBusinessOperations);
            BSOperationMessage* pMsg =  new BSOperationMessage;
            if(pMsg){
                pMsg->m_bsOperation =  std::move(bsOpr);
                pMsg->setMsgType(MESSAGEEVENT::MSG_BSP2BSOP);
                pMsg->setEvent((int)OPERATIONEVENT::OPE_TASK_DEINIT);
                setLastKnownError("BSCallObject::callTimeOutInd OPERATIONEVENT::OPE_TASK_DEINIT Rquested");
                return pMsg;
            }else{setLastKnownError("Business Message crearion error callTimeOutInd");}
        }else{setLastKnownError("Business Operation creatin error callTimeOutInd");}
    }else{setLastKnownError("Call In Connected State");}
    
    return nullptr;
}


    

bool BSCallObject::checkCallStateUpdate(std::unique_ptr<BSOperation> opr){
    setLastKnownError("BSCallObject::checkCallStateUpdate()");

    bool retVal = true;
    if(!opr)
        return false;
    if(!opr->getInParam()){
        return false;
    }
    if(!m_pBusinessOperations){
        m_pBusinessOperations = new BSOperation(getUid());
    }
    switch(getCallState()){
        case BSCallState::STATE_PROCESSING:
            setCallState(opr->getInParam()->getState());
            copyBSOperation(m_pBusinessOperations,opr.get());
        break;
        case BSCallState::STATE_TERMINATE:
            setCallState(opr->getInParam()->getState());
            copyBSOperation(m_pBusinessOperations,opr.get());
        break;
        case BSCallState::STATE_CONNECTED:
        case BSCallState::STATE_ALERTING:
        case BSCallState::STATE_PROCEEDING:
        case BSCallState::STATE_DIALING:
        if(opr->getInParam()->getState() == BSCallState::STATE_TERMINATE || 
            opr->getInParam()->getState() == BSCallState::STATE_CLEAR){
                setCallState(BSCallState::STATE_TERMINATE);
                
            }
            copyBSOperation(m_pBusinessOperations,opr.get());
            break;
        default:
        case BSCallState::STATE_CLEAR:
        case BSCallState::STATE_DISCONNECTED:
            setCallState(BSCallState::STATE_CLEAR);
            
        break;
        
    }
    return retVal;

}


bool BSCallObject::copyBSOperation(BSOperation* destOpr, BSOperation* srcOpr){
  /*  if(srcOpr == nullptr){
        return false;
    }
    if(destOpr == nullptr){
        return false;
    }
    if(srcOpr->getInParam()){
        std::unique_ptr<BSParam> bsp(new BSParam());
        bsp->updateBSParam(*(srcOpr->getInParam()));
        destOpr->updateInParam(std::move(bsp));
    }

    destOpr->setLastKnownError(srcOpr->getLastKnownError());
    destOpr->setUuid(srcOpr->getUuid());
    destOpr->setSipErrorCode(srcOpr->getSipErrorCode());
    destOpr->setOperationEvent(srcOpr->getOperationEvent());
    destOpr->setTaskId(srcOpr->getTaskID());
    destOpr->setCallConnectedStatus(srcOpr->getCallConnectedStatus());
    destOpr->setCallTimeOut(srcOpr->getstrCallTimeout());
    destOpr->setHttpOutput(srcOpr->getHttpOutput());
    destOpr->setHttpStatusCode(srcOpr->getHttpStatusCode());
    destOpr->setConnectedTime(srcOpr->getConnectedTime());
    destOpr->setDisconnectedtime(srcOpr->getDisconnectedTime());
    destOpr->copybsTask(srcOpr->getbsTaskList());
   // destOpr->updateTaskList(m_appTaskList);
    destOpr->m_app.copy(srcOpr->m_app);*/
    return true;

}

