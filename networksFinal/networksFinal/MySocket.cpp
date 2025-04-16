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
    bTCPConnect = false;
    WelcomeSocket = INVALID_SOCKET;
    ConnectionSocket = INVALID_SOCKET;
    
    memset(&SvrAddr, 0, sizeof(SvrAddr));

    //allocate buffer
    if (bufferSize > 0) {
        MaxSize = bufferSize;
        Buffer = new char[bufferSize];
    } else {
        MaxSize = DEFAULT_SIZE;
        Buffer = new char[DEFAULT_SIZE];
    }
    //start winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cout << "ERROR: Failed to start WSA" << std::endl;
        return;
    }

    //address struct
    SvrAddr.sin_family = AF_INET;
    SvrAddr.sin_port = htons(newport);
    SvrAddr.sin_addr.s_addr = inet_addr(newIP.c_str());

    //tcp
    if (connectionType == TCP) {
        if (stype == SERVER) {
            WelcomeSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            if (WelcomeSocket == INVALID_SOCKET) {
                std::cout << "ERROR: TCP Server socket creation failed" << std::endl;
                WSACleanup();
                return;
            }

            if (bind(WelcomeSocket, (SOCKADDR*)&SvrAddr, sizeof(SvrAddr)) == SOCKET_ERROR) {
                std::cout << "ERROR: TCP bind failed" << std::endl;
                closesocket(WelcomeSocket);
                WSACleanup();
                return;
            }

            if (listen(WelcomeSocket, SOMAXCONN) == SOCKET_ERROR) {
                std::cout << "ERROR: TCP listen failed" << std::endl;
                closesocket(WelcomeSocket);
                WSACleanup();
                return;
            }
        }
        else if (stype == CLIENT) {
            //tcp client will create socket & connect later
        }
    }
    //udp
    else if (connectionType == UDP) {
        ConnectionSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (ConnectionSocket == INVALID_SOCKET) {
            std::cout << "ERROR: UDP socket creation failed" << std::endl;
            WSACleanup();
            return;
        }

        if (stype == SERVER) {
            if (bind(ConnectionSocket, (SOCKADDR*)&SvrAddr, sizeof(SvrAddr)) == SOCKET_ERROR) {
                std::cout << "ERROR: UDP bind failed" << std::endl;
                closesocket(ConnectionSocket);
                WSACleanup();
                return;
            }
        }

        //save socket into welcomesocket in case the code later checks it
        WelcomeSocket = ConnectionSocket;
    }
}


//destructor
MySocket::~MySocket()
{
    closesocket(WelcomeSocket);
    closesocket(ConnectionSocket);
    WSACleanup();
    delete[] Buffer;
}

//establish 3-way handshake
void MySocket::ConnectTCP()
{
    //dont make connection for UDP
    if (connectionType == UDP) return;

    if (mySocket == SERVER) {

        //accept incoming connection
        int addrSize = sizeof(SvrAddr);
        ConnectionSocket = accept(WelcomeSocket, (SOCKADDR*)&SvrAddr, &addrSize);
        if (ConnectionSocket == INVALID_SOCKET) {
            std::cout << "ERROR: Accept failed" << std::endl;
            return;
        }

        bTCPConnect = true;
        std::cout << "Client connected!" << std::endl;
    }
    else if (mySocket == CLIENT) {
        //create the socket
        ConnectionSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (ConnectionSocket == INVALID_SOCKET) {
            std::cout << "ERROR: Socket creation failed" << std::endl;
            return;
        }

        //connect to server
        if (connect(ConnectionSocket, (SOCKADDR*)&SvrAddr, sizeof(SvrAddr)) == SOCKET_ERROR) {
            std::cout << "ERROR: Connect failed" << std::endl;
            closesocket(ConnectionSocket);
            return;
        }

        bTCPConnect = true;
        std::cout << "Connected to server!" << std::endl;
    }
}

//close the tcp
void MySocket::DisconnectTCP()
{
    if (bTCPConnect) {
        closesocket(ConnectionSocket);
        ConnectionSocket = INVALID_SOCKET;
        bTCPConnect = false;
        std::cout << "TCP connection closed." << std::endl;
    }
}

void MySocket::SendData(const char* data, int length)
{
    if (connectionType == TCP && bTCPConnect) {
        send(ConnectionSocket, data, length, 0);
    }
    else if (connectionType == UDP) {
        cout << "\nsending with mySocket: " << mySocket << " and connectionType: " << connectionType << "\n";
        sendto(ConnectionSocket, data, length, 0, (SOCKADDR*)&SvrAddr, sizeof(SvrAddr));
    }
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
