#include "stdafx.h"

//#include "ace/INET_Addr.h"
//#include "ace/SOCK_Acceptor.h"
//#include "ace/SOCK_Connector.h"
//#include "ace/SOCK_Stream.h"
//#include "ace/Reactor.h"
//#include "ace/Synch.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/ioctl.h>	//linux porting

#include "ConServer.h"
#include "log.h"
#include "commonutil.h"

#define _snprintf   snprintf    //linux porting

#define OP_UPDATE   0       // just for test
#define OP_ITEM     1
#define OP_NOTIFY   2
#define OP_GET      3
#define OP_GETALL   4

#define DELIMITER   0
#define MAX_LEN     2048


/* Define this to enable SocketReceiver to send line updates in block send, 
   instead on sending each line with send() syscall.
 */
#define SEND_BLOCK_SEND

/* Define this to set socket to use non-blocking IO.
 */
#define SEND_NO_DELAY

/* Define this to delay each notify from server to SEND_NOTIFY_INTERVAL ms,
   This way the interval of each notify sending is controlled by server instead
   of by client, and can save some packets when client and server resides on
   a pretty fast network (the default delay in client is about 100 ms).
 */
#define DELAY_NOTIFY
#define SEND_NOTIFY_INTERVAL	1000


//--------------------------------------------
static const char __modname__[] = "Console";
#define THISMODULE		__modname__

static unsigned long GetTickCount()
{
  struct timeval tv;
  if( gettimeofday(&tv, NULL) != 0 )
  return 0;

  return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}


char *new_strdup(const char *msg)
{
    size_t slen = strlen(msg);
    char *buffer = new char[slen+1];
    memcpy(buffer,msg, slen+1);
    return buffer;
}


///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
class Line
{
public :
    Line(unsigned,short,short,short,const char*);
    Line(unsigned seq, short line, short color);
    ~Line();
    
    int Encode(char*,int maxlen);
    
public :
    bool     mEmptyLine;
    Line*    mpNext;
    Line*    mpPrev;
    int	     mSeq;
    unsigned mTask;
    unsigned mLine;
    unsigned mColor;
    char*    mText;
};


///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
class Lines
{
public :
    Lines(unsigned line_count);
    ~Lines();
    
    Line* UpdateLine(unsigned,short,short,const char*);
    Line* UpdateLine(short line,short color);
    void Get(SocketReceiver*, int);
    void GetAll(SocketReceiver*);
    
private :
    void AddLine(Line *line);
    void MakeTail(Line *line);
    
private :
    int      mSeq;
    unsigned mLineCount;
    Line**   mpIndex;
    Line*    mpHead;
    Line*    mpTail;
};

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
//class SocketReceiver : public ACE_Event_Handler 
class SocketReceiver
{
public:
    SocketReceiver(ConsoleServer* server);

    ~SocketReceiver() { }
    
    int  get_handle(void) const {
	return mpStrm;
    }
    
    virtual int handle_input(ACE_HANDLE fd = ACE_INVALID_HANDLE);

    virtual int handle_close(ACE_HANDLE handle,ACE_Reactor_Mask close_mask) {
	ACE_INET_Addr remote_addr;
	//mpStrm->get_remote_addr(remote_addr);
	CLog::Detail(THISMODULE,"Connection to %s:%d closed", remote_addr.get_host_addr(), remote_addr.get_port_number());
	/* ACE_Reactor::instance()->remove_handler(this, ACE_Event_Handler::RWE_MASK|ACE_Event_Handler::DONT_CALL);
        */
	//delete mpStrm;
	//mpServer->Unregister(this);
	//delete mpBuf;
	delete this;
	return 0;
    }
   
/* 
    const char* GetRemoteAddr() {
	struct in_addr remote_addr;
	get_remote_addr(remote_addr);
        return remote_addr.get_host_addr();
    }
*/
    
    void SendString(const char*, unsigned len);
    void RecvString(const char*, unsigned len);


    void PutNotification(const char *notification, size_t len);
    void SendNotification();

public:
    bool                mNotified;

private:
    char*               mpBuf;
    unsigned            mLen;
    int                 mpStrm;
    ConsoleServer*      mpServer;

    char		mNotificationCache[64];
    size_t		mNotificationCacheLen;
};


///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
class SocketAcceptor : public ACE_Event_Handler 
{
public:
    bool isOpen;
    
    SocketAcceptor(ConsoleServer* server) : mpServer(server) { 
	// do nothing
    }

    ~SocketAcceptor() {
	// do nothing
    }
    
    bool Open(const char* address, unsigned port) {
	if (mPeerAcceptor.open(ACE_INET_Addr(port, address), 5) == -1) {
	    CLog::Error(THISMODULE,"Console acceptor open error");
	    return false;
	}
	CLog::Detail(THISMODULE,"Console acceptor waiting at port %d", port);
	return true;
    }
    
    virtual ACE_HANDLE get_handle(void) const {
	return mPeerAcceptor.get_handle();
    }
    
    virtual int handle_input(ACE_HANDLE fd = ACE_INVALID_HANDLE) 
    {
	ACE_SOCK_Stream *stream = new ACE_SOCK_Stream();

	if (mPeerAcceptor.accept(*stream) == -1) {
	    delete stream;
	    return -1;
	}

	ACE_INET_Addr remote_addr;
	stream->get_remote_addr(remote_addr);
	CLog::Detail(THISMODULE,"Received new connection from %s:%d", remote_addr.get_host_addr(), remote_addr.get_port_number());

	//SocketReceiver * server = new SocketReceiver(stream, mpServer);
	SocketReceiver * server = new SocketReceiver(mpServer);
	/* ACE_Reactor::instance()->register_handler(server, ACE_Event_Handler::READ_MASK);
        */

	/* set TCP socket stream to non-blocking send */
#ifdef SEND_NO_DELAY
	int sock = stream->get_handle();
	/*
	BOOL nodelay = TRUE;
	if (setsockopt(sock, SOL_SOCKET, TCP_NODELAY, (char*)&nodelay, sizeof(nodelay)))
	    CLog::Debug (THISMODULE, "Unable to set socket to nonblocking send");
	*/

	u_long nonbio = 1;
	//if (ioctlsocket(sock, FIONBIO, &nonbio))
	if (ioctl(sock, FIONBIO, &nonbio))
	    CLog::Debug (THISMODULE, "Unable to set socket to nonblocking send");
#endif
	return 0;
    }

    virtual int handle_close(ACE_HANDLE handle,ACE_Reactor_Mask close_mask) {
	ACE_Reactor::instance()->remove_handler(this, ACE_Event_Handler::ACCEPT_MASK|ACE_Event_Handler::DONT_CALL);
	delete this;
	return 0;
    }

private:
    ACE_SOCK_Acceptor	    mPeerAcceptor;
    ConsoleServer*	    mpServer;
};


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
SocketReceiver::SocketReceiver( ConsoleServer* server)
{
    mpBuf = new char[MAX_LEN+1];
    mpBuf[0] = 0;
    mLen = 0;
    /* mpServer->Register(this); */
}


//int SocketReceiver::handle_input(ACE_HANDLE fd)
int SocketReceiver::handle_input(ACE_HANDLE fd)
{
    char buf[1024+1];
    //int len = mpStrm->recv(buf, 1024);
    int len = read(fd,buf,1024);
    if (len <= 0) return -1;
    
    buf[len] = 0;
    RecvString(buf, len);
    return 0;
}


void SocketReceiver::SendString(const char* buf, unsigned len)
{
    //write(buf, len);
}


void SocketReceiver::RecvString(const char* buf, unsigned len)
{
    memcpy(&mpBuf[mLen], buf, len);
    mLen += len;
    
    char* p;
    while(true)
    {
        mpBuf[mLen] = DELIMITER;            // sentinel
        p = strchr(mpBuf, DELIMITER);       // at least got sentinel
        if (p==mpBuf + mLen) break;
	
        //mpServer->RecvString(this, mpBuf, strlen(mpBuf) + 1);
        mLen -= (strlen(mpBuf) + 1);
        memcpy(mpBuf, p+1, mLen);
    }
}

void SocketReceiver::PutNotification (const char *notification, size_t len)
{
#ifdef DELAY_NOTIFY
    /* Don't send notification now.
       If we don't have notification in our cache, put the notification there.
       The notification itself will be sent periodically by ConsoleServer::Timer.
     */
    if (mNotified == false) {
	assert (len < sizeof(mNotificationCache));
	if (len > sizeof(mNotificationCache))
	    return;

	memcpy(mNotificationCache, notification, len);
	mNotificationCacheLen = len;
	mNotified = true;
    }
#else
    memcpy(mNotificationCache, notification, len);
    mNotificationCacheLen = len;
    SendNotification();
#endif
}

void SocketReceiver::SendNotification()
{
#ifdef DELAY_NOTIFY
    if (mNotificationCacheLen) {
	SendString(mNotificationCache, mNotificationCacheLen);
	mNotificationCacheLen = 0;
	mNotified = false;
    }
#else
    SendString(mNotificationCache, mNotificationCacheLen);
    mNotified = true;
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
Line::Line(unsigned seq, short task, short line, short color,const char* text)
: mpNext(0)
, mpPrev(0)
{
    mEmptyLine = false;
    mSeq = seq;
    mTask = task;
    mLine = line;
    mColor = color;
    mText = new_strdup(text);
}


Line::Line(unsigned seq, short line, short color)
: mpNext(0)
, mpPrev(0)
{
    mSeq = seq;
    mEmptyLine = true;
    mLine = line;
    mColor = color;
    mText = NULL;
}


int Line::Encode(char* buffer, int maxlen)
{
    if (!mEmptyLine) 
	return _snprintf(buffer,maxlen,"%u,%u,%u,%u,%u,%s\0", OP_ITEM, mSeq, mTask, mLine, mColor, mText);
    else 
	return _snprintf(buffer,maxlen,"%u,%u,,%u,%u,\0",OP_ITEM,mSeq,mLine,mColor);
}


Line::~Line()
{
    delete [] mText;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
Lines::Lines(unsigned line_count)
: mSeq(0)
, mLineCount(line_count)
{
    mpIndex = new Line*[mLineCount];
    for(unsigned i=0; i<mLineCount; i++) mpIndex[i] = 0;
    mpHead = 0;
    mpTail = 0;
}


Lines::~Lines()
{
    Line* line = mpHead;
    Line* next;
    
    while(line)
    {
        next = line->mpNext;
        delete line;
        line = next;
    }
    delete mpIndex;
}


void Lines::AddLine(Line *line)
{
    if (mpHead==0)
    {
        mpHead = line;
    }
    else
    {
        mpTail->mpNext = line;
        line->mpPrev = mpTail;
    }
    mpTail = line;
}


void Lines::MakeTail(Line *line)
{
    if (line==mpTail) return;
    
    if (line==mpHead)
    {
        mpHead = line->mpNext;
        mpHead->mpPrev = 0;
    }
    else
    {
        if (line->mpPrev) line->mpPrev->mpNext = line->mpNext;
        if (line->mpNext) line->mpNext->mpPrev  = line->mpPrev;
    }
    
    mpTail->mpNext = line;
    line->mpPrev = mpTail;
    line->mpNext = 0;
    mpTail = line;
}


Line* Lines::UpdateLine(unsigned task, short line, short color, const char* text)
{
    if (line>=mLineCount) return 0;
    
    if (mpIndex[line]==0)
    {
        //**CLog::Detail(THISMODULE,"%05d %6s t=%05d line=%03d col=%02d text={%s}", mSeq, "NEW", task, line, color, text);
        mpIndex[line] = new Line(mSeq, task, line, color, text);
        AddLine(mpIndex[line]);
    }
    else
    {
        //**CLog::Detail(THISMODULE,"%05d %6s t=%05d line=%03d col=%02d text={%s}", mSeq, "UPD", task, line, color, text);
	mpIndex[line]->mEmptyLine = false;
        mpIndex[line]->mSeq = mSeq;
        mpIndex[line]->mTask = task;
        mpIndex[line]->mLine= line;
        mpIndex[line]->mColor = color;
        delete [] mpIndex[line]->mText;
        mpIndex[line]->mText = new_strdup(text);
        MakeTail(mpIndex[line]);
    }
    mSeq++;
    
    return mpIndex[line];
}


Line* Lines::UpdateLine(short line,short color)
{
    if (line>=mLineCount) return 0;
    
    if (mpIndex[line]==0)
    {
        //**CLog::Detail(THISMODULE,"%05d %6s line=%03d col=%02d", mSeq, "CLR", line, color);
        mpIndex[line] = new Line(mSeq,line,color);
        AddLine(mpIndex[line]);
    }
    else
    {
        //**CLog::Detail(THISMODULE,"%05d %6s line=%03d col=%02d", mSeq, "CLR", line, color);
	mpIndex[line]->mEmptyLine = true;
        mpIndex[line]->mSeq = mSeq;
        mpIndex[line]->mLine= line;
        mpIndex[line]->mColor = color;
        MakeTail(mpIndex[line]);
    }
    mSeq++;
    
    return mpIndex[line];
}


void Lines::Get(SocketReceiver* sr, int start)
{
    /* Packet buffer, the size is approximately equal to MTU.
     */
    static char packet [1400];
    char linebuffer[256];
    size_t pktsize;
    
    unsigned c = 0;
    Line* line = mpHead;
    pktsize = 0;

    /* Encode each line into a packet buffer, send the packet buffer
       to the network only when it is full.
     */
    while(line)
    {
        if (line->mSeq>start)
        {
	    int printed = line->Encode (linebuffer, sizeof(linebuffer));
            if (printed >= 0) {
		++printed; // plus delimiter(0)
#ifdef SEND_BLOCK_SEND
		if (pktsize + printed < sizeof(packet)) {
		    memcpy(packet+pktsize, linebuffer, printed); 
		    pktsize += printed;
		} else {
		    sr->SendString(packet, pktsize); 
		    memcpy (packet, linebuffer, printed);
		    pktsize = printed;
		}
#else
		sr->SendString(linebuffer, printed); 
#endif
	    }
            
            c++;
        }
        line = line->mpNext;
    }
    if (pktsize > 0)
	sr->SendString(packet, pktsize); 
}


void Lines::GetAll(SocketReceiver* sr)
{
    Get(sr, -1);
}




///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
/*
ConsoleServer::ConsoleServer(unsigned port, const char* address, unsigned line_count) : 
    mPort(port),
    _timeout(0,0),
    mOpened(false),
    mLastSendNotify(0)
{
    mAddress = new_strdup(address);
    mpAcceptor = new SocketAcceptor(this);
    _reactor = ACE_Reactor::instance();
    unsigned error_code;
    Open(error_code);
    
    mpLines = new Lines(line_count);
}
*/

ConsoleServer::ConsoleServer(unsigned port, const char* address, unsigned line_count)
{
    mAddress = new_strdup(address);
    unsigned error_code;
    bzero((char *) &console_server, sizeof(console_server));
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    printf("Creating socket in ConsoleServer::ConsoleServer() =: %d\n", sockfd); //rkv
    if (sockfd < 0)
        printf("ERROR opening socket");
    console_server.sin_family = AF_INET;
    console_server.sin_addr.s_addr = INADDR_ANY;
    if (bind(sockfd, (struct sockaddr *) &console_server, sizeof(console_server)) < 0)
       printf("ERROR on binding");

    listen(socket,5);

    Open(error_code);

    mpLines = new Lines(line_count);
}

ConsoleServer::~ConsoleServer()
{
    delete mpLines;
    delete [] mAddress;

    /* mpAcceptor->handle_close(mpAcceptor->get_handle(), 0);

    std::list<SocketReceiver*>::iterator p = mlClient.begin();
    std::list<SocketReceiver*>::iterator end = mlClient.end();
    while (p != end) {
	SocketReceiver* receiver = *p++;
	receiver->handle_close(0,ACE_Event_Handler::RWE_MASK);
    }
    */
    
    mpLines = NULL;
    mAddress = NULL;
}

/*
bool ConsoleServer::Open(unsigned& errorCode)
{
    errorCode = 0;
    if (mOpened) return true;
    
    if (!mpAcceptor->Open(mAddress, mPort)) {
	return false;
    }
    _reactor->register_handler(mpAcceptor,ACE_Event_Handler::ACCEPT_MASK);
    mOpened = true;
    return true;
}
*/

bool ConsoleServer::Open(unsigned& errorCode)
{
    errorCode = 0;
    //if (mOpened) return true;
    
    int  clilen = sizeof(mlClient);
    sockfd = accept(sockfd, (struct sockaddr *) &mlClient, &clilen);
    if (sockfd < 0)
      printf("ERROR on accept");

    mOpened = true;
    return true;
}

void ConsoleServer::RecvString(const char* str, unsigned len)
{
    //CLog::Detail(THISMODULE,"RecvString");
    //ACE_Guard<ACE_Token>    g(mToken);

    static char buf[1024];
    //if (len==0) len = strlen(str);
    //strncpy(buf, str, len);
    //buf[len] = 0;
    //if (buf[len-1]=='\n') buf[len-1] = 0;
    //CLog::Detail(THISMODULE,"(%-15s) RECV %d{%s}", sr->GetRemoteAddr(), len, buf);

    short op = atoi(str);    
    if (op==OP_UPDATE) {
        //RecvStringUpdate(sr, str, len);
        RecvStringUpdate( str, len);
        return;
    }
    
    char *p1 = strchr(str, ',');
    if (op==OP_GET) {
        if (p1==0) return;
        int start = atoi(p1+1);    
	
       // mpLines->Get(sr, start);
#ifndef DELAY_NOTIFY
        sr->mNotified = false;
#endif
    } else if (op==OP_GETALL) {
	//mpLines->GetAll(sr);
#ifndef DELAY_NOTIFY
        sr->mNotified = false;
#endif
    }
}


void ConsoleServer::RecvStringUpdate( const char* str, unsigned len)
{
    char* p = strchr(str, ',');
    
    if (p==0) return;
    long task_id = atoi(p+1);    
    
    p = strchr(p+1, ',');
    if (p==0) return;
    long line = atoi(p+1);    
    
    p = strchr(p+1, ',');
    if (p==0) return;
    long color = atoi(p+1);
    
    p = strchr(p+1, ',');
    if (p==0) return;
    
    Display(task_id, line, color, p+1);
}



void ConsoleServer::Display(unsigned taskid, short line, short color, const char* text)
{
    //ACE_Guard<ACE_Token>    g(mToken);
    
    Line* i = mpLines->UpdateLine(taskid, line, color, text);
    if (i==0) return;
    /* Notify(i);     */
}


void ConsoleServer::ClearDisplay(short line, short color)
{
    //ACE_Guard<ACE_Token>    g(mToken);
    
    Line* i = mpLines->UpdateLine(line,color);
    if (i==0) return;
    /* Notify(i); */

}


void ConsoleServer::Timer()
{
    //_reactor->handle_events(_timeout);

#ifdef DELAY_NOTIFY
    /* Ask all SocketReceiver to send their delayed notifications only
       when SEND_NOTIFY_INTERVAL elapsed.
     */
    if (GetTickCount() - mLastSendNotify >= SEND_NOTIFY_INTERVAL) {
#if 0
	std::list<SocketReceiver*>::iterator p = mlClient.begin(), end=mlClient.end();
	for (; p!=end; ++p) {
	    SocketReceiver* receiver = *p;
	    receiver->SendNotification();
	}
#endif
	mLastSendNotify = GetTickCount();
    }
#endif
}

/*
void ConsoleServer::Register(SocketReceiver* sr)
{
    mlClient.push_back(sr);
}


void ConsoleServer::Unregister(SocketReceiver* sr)
{
    mlClient.remove(sr);
}


void ConsoleServer::Notify(Line* line)
{
    if (mlClient.size()==0) return;
    static char notify[32];
    
    std::list<SocketReceiver*>::iterator p = mlClient.begin();
    std::list<SocketReceiver*>::iterator end = mlClient.end();
    
    int notifylen = _snprintf(notify,sizeof(notify),"%d,%d", OP_NOTIFY, line->mSeq);
    if (notifylen < 1)
	return;

    for(; p!=end; ++p)
    {
	SocketReceiver *sr = *p;
	sr->PutNotification (notify, notifylen+1);  // include delimiter (0)
    }
}
*/
