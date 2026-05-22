#include "CAUtilites.h"
using namespace std;
enum class ClientProtoType: unsigned int {
    IPv4 = 0,  // IPv4 protocol
    IPv6 // IPv6 protocol
};
enum class ClientSocketType: unsigned int{
    TCP_STREAM = 0, // for TCP (reliable, connection oriented)
    UDP_STREAM // for UDP (unreliable, connectionless)
};
class CAclientSocket
{
    private:
        int client;
        std::string address;
        int port;
        struct sockaddr_in server;
    public:
        std::string reply;
        int protocaltype,sockettype;
        CAclientSocket();
        ~CAclientSocket();
        bool connctclient(std::string address, int port,ClientProtoType clientprototype,ClientSocketType clientsockettype);
        bool send_data(std::string data);
        std::string receive(int size);
        bool CloseSocket();

};