#ifndef CA_FILE_NOTIFIER
#define CA_FILE_NOTIFIER
#include "CAThread.h"
enum class NOTIFIER_EVENT{
NE_ALLFILEEVENT,
NE_ONLYFILEMODIFY,
};

class CAFileNotifier{
    public:
    CAFileNotifier()=default;
    virtual ~CAFileNotifier()=default;
    std::string getLastKnownError(){return m_lastKnownError;}
    void setLastKnownError(std::string err){ m_lastKnownError= err;};
    CAFileNotifier(std::string fileName,  NOTIFIER_EVENT evtType, CA::CAThread* Requester, int MsgType);
    
    CA::CAThread* getRequesterThread(){return m_threadReq;}
    int getEventMsgType(){return m_eventMsgType;}
    NOTIFIER_EVENT getNotifyEventType(){return m_eventType;}
    std::string getFileNamePath(){return m_fileNameWithPath;}

    void startNotification();
    
    
    
    private:
    
    std::string m_lastKnownError;
    std::string m_fileNameWithPath;
    NOTIFIER_EVENT m_eventType;
    int m_eventMsgType;
    CA::CAThread* m_threadReq;
    
    
    
    
    
    
    //void stopNotification();


};

#endif