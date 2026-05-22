/*----------------------------------------------------------------* 
 * $Archive: /xGate/automation.h $
 * $Date: 1/25/05 10:53a $
 *
 *  XGATE AUTOMATION
 *
 * $Revision: 4 $
 * $History: automation.h $
 * 
 * *****************  Version 4  *****************
 * User: Bennylp      Date: 1/25/05    Time: 10:53a
 * Updated in $/xGate
 * Fixed compilation errors.
 * 
 * *****************  Version 3  *****************
 * User: Sonny        Date: 24/05/04   Time: 11:11
 * Updated in $/xGate
 * Initial stable version
 * 
 */
#if !defined __XGATE_VTV_AUTOMATION_INCLUDED__
#define __XGATE_VTV_AUTOMATION_INCLUDED__

#if defined XGATE_HAS_VTV_AUTOMATION

#include "ServiceProvider.h"
#include "../vtv/Automation/AS_Lib/as_net.h"
#include "../vtv/Automation/AS_Lib/as_packet.h"

#define AS_PACKET_HEADER_SIZE	    sizeof(AS_Net_Param)
#define AS_PACKET_MAX_SIZE	    64 * 1024


class VTVA_Server;
class VTVA_Client_Control;
class VTVA_API;

/*******************************************************************************/
class VTVA_Link : public ACE_Event_Handler
{
public:
    VTVA_Link(ACE_Reactor *reactor);
    virtual ~VTVA_Link();

    bool open(const char *host, int port);
    bool accept(ACE_SOCK_Acceptor *acceptor);
    void close();

    bool send(const char *s);
    bool send(AS_Net_Param *cmd);

    virtual void on_message(char *) = 0;
    virtual void on_opened() = 0;
    virtual void on_closed() = 0;

    bool opened() { return opened_; }

protected:
    virtual ACE_HANDLE get_handle (void) const { return stream_.get_handle(); };
    virtual int handle_input(ACE_HANDLE fd=ACE_INVALID_HANDLE);
    virtual int handle_output(ACE_HANDLE fd=ACE_INVALID_HANDLE); 
    virtual int handle_exception(ACE_HANDLE fd=ACE_INVALID_HANDLE);
    virtual int handle_timeout(const ACE_Time_Value &tv,const void *arg);
    virtual int handle_close(ACE_HANDLE handle,ACE_Reactor_Mask close_mask);

    ACE_SOCK_Stream stream_;
    bool opened_, registered_;
    ACE_Reactor *reactor_;

    int len_rcv_;

    char buffer_[AS_PACKET_MAX_SIZE * 10];

    void handle_data(char *, int len);

    void reactor_register();
    void reactor_unregister();
};


/*******************************************************************************/
class VTVA_Server_Link : public VTVA_Link
{
public:
    VTVA_Server_Link(ACE_Reactor *reactor);
    ~VTVA_Server_Link();

protected:
    virtual void on_message(char *);
    virtual void on_opened();
    virtual void on_closed();

private:
    VTVA_Client_Control *controller_;

    void attach_controller(VTVA_Client_Control *controller) { controller_ = controller; }
    void detach_controller() { controller_ = NULL; }

    friend class VTVA_Client_Control;
};


/*******************************************************************************/
class VTVA_Client_Control
{
public:
    VTVA_Client_Control(VTVA_Server_Link *link);
    virtual ~VTVA_Client_Control();

    bool send_msg(AS_Net_Param *);

protected:
private:
    VTVA_Server_Link *link_;

    void on_link_opened() {}
    void on_link_closed();
    void on_message(char *);

    friend class VTVA_Server_Link;
};


/*******************************************************************************/
class VTVA_Server_Acceptor : public ACE_Event_Handler
{
public:
    VTVA_Server_Acceptor(VTVA_Server *, int);
    virtual ~VTVA_Server_Acceptor();
    bool accept(ACE_SOCK_Stream &stream);

protected:
    virtual ACE_HANDLE get_handle (void) const { return acceptor_.get_handle(); };
    virtual int handle_input(ACE_HANDLE fd=ACE_INVALID_HANDLE);
    virtual int handle_output(ACE_HANDLE fd=ACE_INVALID_HANDLE) { return 0; }; 
    virtual int handle_exception(ACE_HANDLE fd=ACE_INVALID_HANDLE) { return 0; };
    virtual int handle_timeout(const ACE_Time_Value &tv,const void *arg) { return 0; };
    virtual int handle_close(ACE_HANDLE handle,ACE_Reactor_Mask close_mask) { return 0; };

private:
    VTVA_Server *const server_;
    ACE_SOCK_Acceptor acceptor_;
};


/*******************************************************************************/
class VTVA_Server : public ServiceProvider
{
public:
    static void Create() { instance_ = new VTVA_Server(); }
    static VTVA_Server * Instance() { return instance_ ; }
    static void Destroy() { delete instance_; instance_ = NULL; }

protected:
private:
    struct VTVA_Msg {
	CTask *task;
	int msg;
	VTVA_Client_Control *controller;
	char data[AS_PACKET_MAX_SIZE];
    };

    struct VTVA_Task {
	CTask *task;
	std::list<int> devices_;
	VTVA_Msg msg_input_;
	VTVA_Msg msg_output_;
	bool is_device_registered(int dev_id) {
	    std::list<int>::iterator i;
	    for (i = devices_.begin(); i != devices_.end(); i++) {
		if (*i == dev_id)
		    return true;
	    }
	    return false;
	}
    };


    static VTVA_Server *instance_;
    VTVA_API *api_;
    VTVA_Server_Acceptor *acceptor1_;
    VTVA_Server_Acceptor *acceptor2_;
    ACE_Reactor reactor_;


    std::list<VTVA_Client_Control *> client_list_;
    std::list<VTVA_Task> task_list_;
    std::list<VTVA_Msg> message_queue_;

    VTVA_Server();
    ~VTVA_Server();

    void register_client(VTVA_Client_Control *client);
    void unregister_client(VTVA_Client_Control *client);

    int register_handler(ACE_Event_Handler *event_handler, ACE_Reactor_Mask mask) {
	return reactor_.register_handler(event_handler, mask);
    }
    int remove_handler(ACE_Event_Handler *event_handler, ACE_Reactor_Mask mask) {
	return reactor_.remove_handler(event_handler, mask);
    }

    virtual void on_message(VTVA_Client_Control *controller, char *);
    void on_client_disconnected(VTVA_Client_Control *controller);


    // xGate framework implementations
    virtual bool Load();
    virtual bool Unload();
    virtual bool AttachTask(CTask *);
    virtual void DetachTask(CTask *);
    virtual void HandleEvents();

    // VTVA API handler
    int create_msg_number();
    CTask * find_task(int dev_id);
    VTVA_Task * find_task(CTask *task);
    VTVA_Msg get_message(CTask *task);
    VTVA_Msg * find_message(CTask *task, int msg);

    STATUS VTVADeviceRegister(CTask *task, int dev_id);

    STATUS VTVAMsgGetInput(CTask* task, int *retval);
    STATUS VTVAMsgGetOutput(CTask* task, int *retval);

    STATUS VTVAMsgGetByte(CTask* task, int *retval, int msg, int offset);
    STATUS VTVAMsgGetShort(CTask* task, int *retval, int msg, int offset);
    STATUS VTVAMsgGetLong(CTask* task, int *retval, int msg, int offset);
    STATUS VTVAMsgGetReal(CTask* task, float *retval, int msg, int offset);
    STATUS VTVAMsgGetString(CTask* task, char *retval, int msg, int offset, int length);

    STATUS VTVAMsgPutByte(CTask* task, int msg, int offset, int value);
    STATUS VTVAMsgPutShort(CTask* task, int msg, int offset, int value);
    STATUS VTVAMsgPutLong(CTask* task, int msg, int offset, int value);
    STATUS VTVAMsgPutReal(CTask* task, int msg, int offset, float value);
    STATUS VTVAMsgPutString(CTask* task, int msg, int offset, const char *value, int length);
    
    STATUS VTVAMsgSend(CTask* task, int msg);


    void on_incoming_control(VTVA_Server_Link *link);


    friend class VTVA_Server_Acceptor;
    friend class VTVA_Client_Control;
    friend class VTVA_API;

};

#endif // XGATE_HAS_VTV_AUTOMATION

#endif // __XGATE_VTV_AUTOMATION_INCLUDED__

