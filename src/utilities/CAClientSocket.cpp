#include "CAClientSocket.h"
CAclientSocket::CAclientSocket()
{
    client = -1;
    port = 0;
    address = "";
}

CAclientSocket::~CAclientSocket()
{
}
/* Method Name: connect 
Description:  This function will help to established the connection to server from client
Parameters:  IP Address and port needed
return:  Returns as a boolean as per the connection establishment
*/

bool CAclientSocket::connctclient(std::string address , int port,ClientProtoType clientprototype,ClientSocketType clientsockettype)
{
    switch(clientprototype)
    {
        case ClientProtoType::IPv4:
          protocaltype = AF_INET;
        break;
        case ClientProtoType::IPv6:
          protocaltype = AF_INET6;
        break;
    }
    switch(clientsockettype)
    {
        case ClientSocketType::TCP_STREAM:
          sockettype = SOCK_STREAM;
        break;
        case ClientSocketType::UDP_STREAM:
          sockettype = SOCK_DGRAM;
        break;
    }

    // create socket if it is not already created
    if(client == -1)
    {
        //Create socket
        client = socket(protocaltype , sockettype , 0);
        if (client == -1)
        {
            return false;
        }
    }
    else { /* OK , nothing */ }

    server.sin_addr.s_addr = inet_addr(address.c_str());
    server.sin_family = protocaltype;
    server.sin_port = htons( port );

    if(connect(client,(struct sockaddr *)&server , sizeof(server)) < 0 )
    {
        return false;
    }
    return true;
}

/* Method Name: send_data
Description: this function helps to sent data to the server from client
Parameters:  data as a string
return: Returns as a boolean as per the data send successfully or not
*/

bool CAclientSocket::send_data(std::string data)
{
    if( send(client , data.c_str() , strlen( data.c_str() ) , 0) < 0)
    {
       // cout<<"Send failed : "<<endl;
        return false;
    }else{
         //   cout<<"Data send\n";
        }

    return true;
}

/* Method Name: receive
Description: This function will recive the data from the server once connection was established
Parameters:  size should be as an integer
return: returns as string, and it contains the recived data
*/

std::string CAclientSocket::receive(int size)
{
  char buffer[size];
  string reply;
  //Receive a reply from the server
  if( recv(client , buffer , sizeof(buffer) , 0) < 0)
  {
       //cout<<"recv failed"<<endl;
       return NULL;
  }else{
        //cout<<"data was recived from server"<<endl;
        //cout <<"Data ---- \t"<<endl;
  }
    reply = buffer;
    cout<<reply<<endl;
    return reply;
}

/* Method Name: CloseSocket
Description: This function will close the connection
Parameters:  NA
return: Returns as boolean value
*/

bool CAclientSocket::CloseSocket()
{
  if(client >0)
  {
        close(client);
        //cout<<"The socket was closed"<<endl;
  }else{
        //cout<<"The socket was already closed"<<endl;
        return false;
    }
  return  true;
}

