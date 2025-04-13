#pragma once

#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>

enum SocketType {
	CLIENT, SERVER
};

enum ConnectionType {
	TCP, UDP
};

//size of buffer
const int DEFAULT_SIZE = 1024;



class MySocket {

    char* Buffer;
    SOCKET WelcomeSocket;
    SOCKET ConnectionSocket;
    struct sockaddr_in SvrAddr;
    SocketType mySocket;
    std::string IPAddr;
    int Port;
    ConnectionType connectionType;
    bool bTCPConnect;
    int MaxSize;

public:

    MySocket(SocketType, std::string, unsigned int, ConnectionType, unsigned int);
    ~MySocket();
    void ConnectTCP();
    void DisconnectTCP();
    void SendData(const char*, int);
    int GetData(char*);
    std::string GetIPAddr();
    void SetIPAddr(std::string);
    void SetPort(int);
    int GetPort();
    SocketType GetType();
    void SetType(SocketType);
};