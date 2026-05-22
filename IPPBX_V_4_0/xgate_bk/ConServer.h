#ifndef XGate_h
#define XGate_h


#include <list>
//#include "ace/Token.h"


    ///////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////
 //   class SocketAcceptor;
    class SocketReceiver;
    class Line;
    class Lines;


    ///////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////
    class ConsoleServer
    {
        friend class SocketReceiver;

    public :
        ConsoleServer(unsigned port=8000, const char* address = "0.0.0.0", unsigned line_count = 1024);
        ~ConsoleServer();

    public :
        bool Open(unsigned& errorCode);
        void Display(unsigned taskid, short line, short color, const char* text);
        void ClearDisplay(short line, short color);
        void Timer();

    private :
        void Register(SocketReceiver*);
        void Unregister(SocketReceiver*);
        void Notify(Line*);
        //void RecvString(SocketReceiver* sr, const char* str, unsigned len);
        void RecvString( const char* str, unsigned len);
        //void RecvStringUpdate(SocketReceiver* sr, const char* str, unsigned len);
        void RecvStringUpdate( const char* str, unsigned len);

    private :
        unsigned                    mPort;
        char*                       mAddress;
        //SocketAcceptor*             mpAcceptor;
        //std::list<SocketReceiver*>  mlClient;
        //unsigned                    mNotNotifiedCount;
        bool                        mOpened;
        Lines*                      mpLines;
        //ACE_Token                   mToken;
	//ACE_Time_Value		    _timeout;
	//ACE_Reactor*		    _reactor;
	//DWORD			   mLastSendNotify;
        struct sockaddr_in      console_server,mlClient;
        struct in_addr          ipv4addr;
        struct hostent          *hp;
	unsigned long 		mLastSendNotify;	//linux porting
        int 			sockfd;
    };

#endif
