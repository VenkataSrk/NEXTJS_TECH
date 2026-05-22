#include "CAServerSocket.h"
using namespace std;



/* Method Name: CreateServerSockethandle
Description: this function is used to create connection handle with clients
Parameters:  ip,port,prototype(ipv4 or ipv6), sockettype(tcp or udp)
return: boolean as per the connection result
*/
bool CAServerSocket::CreateServerSockethandle(std::string ipaddr,int port,CAprotocaltype _protocaltype,CAsockettype _sockettype)
{
    switch(_protocaltype)
    {
        case _AF_INET:
          protocaltype = AF_INET;
        break;
        case CAprotocaltype::_AF_INET6:
          protocaltype = AF_INET6;
        break;
    }
    switch(_sockettype)
    {
        case CAsockettype::_SOCK_STREAM:
          sockettype = SOCK_STREAM;
        break;
        case CAsockettype::_SOCK_DGRAM:
          sockettype = SOCK_DGRAM;
        break;
    }
    
    if (socketfd = socket(protocaltype,sockettype, 0)) {
        bzero((char *) &server_addr, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        server_addr.sin_port = htons(port);
        int ret =  bind(socketfd, (struct sockaddr*)&server_addr,sizeof(server_addr));
        if (ret == 0 ){
            size = sizeof(server_addr);
            listen(socketfd, 5);  // we can increment upto 4-5
            // write a loop for socket accpet for many clients
            setSocketStatus(true);
            CA::CAThread serverSock([=]()
            {
             while(getSocketStatus()){   
                clientInfo *client =  new clientInfo();
                if(client){
                    client->socketfd =  accept(socketfd,(struct sockaddr *)&server_addr,&size);
                    if(client->socketfd ){
                        client->ipAddr =   inet_ntoa(server_addr.sin_addr);
                        insertToSocketMap(client);
                        ReciveData(client);
                    }
                }
             }
            });
            return true;
        }
        
    }
    return true;
}

bool CAServerSocket::insertToSocketMap(clientInfo* cI){
    if(cI){
        m_clientInfo[cI->socketfd] =  cI;
        return true;
    }
return false;
}
/* Method Name: ReciveData
Description: this function is to recive the dada ,once connection was establishment with clients
Parameters:  NA
return: NA
*/
void CAServerSocket::ReciveData(clientInfo* cI)
{
    if (cI)
    {   
        CA::CAThread readThread([=](){
            char buffer[256] = {"\0"};
            read(cI->socketfd,buffer,255);
            //send the message to business Processor

        });
    }
}
/* Method Name: SendData
Description: this function is to send data to the client once connection was establishment
Parameters:  data as string
return: NA
*/
void CAServerSocket::SendData(std::string data)
{
    if(server > 0)
    {
        write(server,data.c_str(),bufsize);
        cout<<"successfully send dada to the client"<<endl;
    }else
    {
        cout<<"con't able to send dada to the client"<<endl;
    }
}
/* Method Name: CloseSockethandle
Description: this function is to close the connection with client
Parameters:  NA
return: NA
*/
void CAServerSocket::CloseSockethandle()
{
    if(server > 0)
    {
        cout << "\n\n=> Connection terminated with IP " << inet_ntoa(server_addr.sin_addr);
        close(server);
    }
    close(client);
}

