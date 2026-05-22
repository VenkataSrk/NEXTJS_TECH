#include "CAFileNotifier.h"
#define EVENT_SIZE  ( sizeof (struct inotify_event) )
#define EVENT_BUF_LEN     ( 1024 * ( EVENT_SIZE + 16 ) )
CAFileNotifier::CAFileNotifier(std::string fileName, NOTIFIER_EVENT evtType, CA::CAThread* Requester,int MsgType){
    m_fileNameWithPath = fileName;
    m_eventType =  evtType;
    m_eventMsgType =  MsgType;
    if(Requester){ m_threadReq =  Requester;}
    else{m_threadReq =  nullptr;}
}

void CAFileNotifier::startNotification(){
    int fd =  inotify_init();

    if(fd < 0){
        setLastKnownError("File Descriptor error");
        return;
    }
    unsigned int fdmask;
    switch(getNotifyEventType()){
        case NOTIFIER_EVENT::NE_ALLFILEEVENT:
            fdmask = IN_ACCESS | IN_ATTRIB |IN_MODIFY |IN_OPEN |IN_CLOSE_WRITE ;
            break;
        case NOTIFIER_EVENT::NE_ONLYFILEMODIFY:
            fdmask =  IN_MODIFY | IN_CLOSE_WRITE ;
            break;
       default:
        fdmask = IN_MODIFY;
       break;
    }
    
    int m_watchDir =  inotify_add_watch(fd,getFileNamePath().c_str(),fdmask);
    int msgEvent = getEventMsgType();
    CA::CAThread* th =  getRequesterThread();
    CA::CAThread FSNotifier([fd,th,msgEvent,m_watchDir](){
        char buffer[EVENT_BUF_LEN];
        memset(buffer,'\0',EVENT_BUF_LEN);
        int len = read(fd,buffer,EVENT_BUF_LEN);
        int i = 0 ;
        if(i < len){
            struct inotify_event *event = ( struct inotify_event * ) &buffer[ i ]; 
            #if 0
                if(event->mask & IN_MODIFY ) std::cout <<"IN_MODIFY" << std::endl;
                if(event->mask & IN_ACCESS ) std::cout << "IN_ACCESS" << std::endl;
                if(event->mask & IN_ATTRIB) std::cout << "IN_ATTRIB" << std::endl;
                if(event->mask & IN_CREATE ) std::cout << "IN_CREATE" << std::endl;
                if(event->mask & IN_CLOSE ) std::cout << "IN_CLOSE" << std::endl;
                if(event->mask & IN_CLOSE_WRITE ) std::cout << "IN_CLOSE_WRITE" << std::endl;
                if(event->mask & IN_DELETE ) std::cout << "IN_DELETE" << std::endl;
                if(event->mask & IN_DELETE_SELF) std::cout << "IN_DELETE_SELF" << std::endl;
                if(event->mask & IN_MOVE_SELF) std::cout << "IN_MOVE_SELF" << std::endl;
                if(event->mask & IN_MOVE ) std::cout << "IN_MOVE" << std::endl;
                if(event->mask & IN_OPEN ) std::cout << "IN_OPEN" << std::endl;
                if(event->mask & IN_ISDIR) std::cout << "IN_ISDIR" << std::endl;
            #endif
            if(th){
                std::unique_ptr<CA::CAMessageBlock> pMsg(new CA::CAMessageBlock());
                pMsg->setMsgType(msgEvent);
                th->PostMessage(std::move(pMsg));      
            } 
        }
        inotify_rm_watch(fd,m_watchDir);
        close(fd);
    });
}
            
        


    



