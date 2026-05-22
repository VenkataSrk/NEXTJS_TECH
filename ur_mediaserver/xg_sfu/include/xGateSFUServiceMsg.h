#ifndef XGATE_SFU_SERVICE_MSG
#define XGATE_SFU_SERVICE_MSG

class xGateSFUServiceMsg : public xGateBaseMsg
{
public:
  xGateSFUServiceMsg(void) :
   xGateBaseMsg(IURDefines::UR_MODULE_SFU_SERVICE){};
 
 virtual ~xGateSFUServiceMsg()
 {}
 unsigned int get_connection_id(void){
   return m_connection_id;	 
 }
 
 void set_connection_id(int connection_id)
 {
   m_connection_id = connection_id;  	 
 }
 void set_sfu_id(int sfu_id){
  m_sfu_id = sfu_id;	 
 }
 int get_sfu_id(void){
   return m_sfu_id;
 }
 void set_conn_ip(string connIp){
  m_conn_ip = connIp;
 }
 string get_conn_ip(void){
   return m_conn_ip; 	 
 }	 
 int get_port_number(void){
    return m_port_number;	 
 }
 void set_port_number(int port){
   m_port_number = port;
 }
 char *getMsg(void){
   return data;
 }	 
 void setMsg(char* msg){
    data = msg; 	 
 }	 
 void setMsgLen(int msgLen){
   len = msgLen; 	 
 }
 int getMsgLen(void){
   return len;
 }	 
private:
 int m_connection_id;
 int m_sfu_id;
 string m_conn_ip;
 int m_port_number;
 char *data;
 int len;
};

#endif
