#define _WINSOCK_DEPRECATED_NO_WARNINGS

#pragma comment(lib, "Ws2_32.lib")

#include "MySocket.h"
#include <stdint.h>
#include "PktDef.h"
#include <iostream>

#include <winsock2.h>
#include <ws2tcpip.h>
using namespace std;

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


    
    //wsa startup
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cout << "ERROR: Failed to start WSA" << endl;
        return;
    }


    SvrAddr.sin_family = AF_INET;
    SvrAddr.sin_port = htons(newport);
    SvrAddr.sin_addr.s_addr = inet_addr(newIP.c_str());

    //set up tcp or udp sockets to receive
    if (ctype == TCP) {
        



    }
    else {


    }

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


int MySocket::GetData(char* dest)
{
    int length = (unsigned int)Buffer[3] + HEADERSIZE + 1;//4th position in buffer is packet length
    memcpy(dest, Buffer, length);
    return length;
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
