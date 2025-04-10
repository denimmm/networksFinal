#include "MySocket.h"

//constructor
MySocket::MySocket(SocketType stype, std::string newIP, unsigned int newport, ConnectionType ctype, unsigned int bufferSize)
{
    mySocket = stype;
    IPAddr = newIP;
    Port = newport;
    connectionType = ctype;

    if (bufferSize > 0) {
        MaxSize = bufferSize;
        Buffer = new char[bufferSize];
    }
    else {
        MaxSize = DEFAULT_SIZE;
        Buffer = new char[DEFAULT_SIZE];
    }

    //set up tcp or udp sockets to receive


}

//destructor
MySocket::~MySocket()
{
    delete Buffer;
}

//establish 3-way handshake
void MySocket::ConnectTCP()
{
    //dont make connection for UDP
    if (connectionType == UDP) {
        return;
    }



}

//close the tcp
void MySocket::DisconnectTCP()
{

}

void MySocket::SendData(const char*, int)
{

}

int MySocket::GetData(char*)
{

}

std::string MySocket::GetIPAddr()
{
    return IPAddr;
}

void MySocket::SetIPAddr(std::string newIP)
{
    IPAddr = newIP;
}

void MySocket::SetPort(int newport)
{
    Port = newport;
}

int MySocket::GetPort()
{
    return Port;
}

SocketType MySocket::GetType()
{
    return mySocket;
}

void MySocket::SetType(SocketType type)
{
    mySocket = type;
}
