#pragma once
#include <stdint.h>

const int FORWARD = 1;
const int BACKWARD = 2;
const int LEFT = 3;
const int RIGHT = 4;
const int HEADERSIZE = 4;

enum CmdType {
    DRIVE = 0, SLEEP = 1, RESPONSE = 2, UNKOWN =3
};

struct Header {

    unsigned short PktCount;

    uint8_t Drive : 1;
    uint8_t Status : 1;
    uint8_t Sleep : 1;
    uint8_t Ack : 1;

    uint8_t Padding : 4;

    unsigned char Length;

};

struct DriveBody {
    unsigned char direction;
    unsigned char duration;
    unsigned char speed;
};

struct StatusBody {
    unsigned short int LastPktCounter;
    unsigned short int CurrentGrade;
    unsigned short int HitCount;

    unsigned char LastCmd;
    unsigned char LastCmdValue;
    unsigned char LastCmdSpeed;
};

struct CmdPacket {
    Header* header;
    char* data;
    unsigned char CRC;
};

class PktDef {

    CmdPacket* cmdPacket;
    char* RawBuffer;

public:
    PktDef(void);
    PktDef(char* incomingPacket);

    void SetCmd(CmdType type);
    void SetBodyData(char* bodyData, int dataSize);
    void SetPktCount(int count);

    CmdType GetCmd();
    bool GetAck();
    int GetLength();
    char* GetBodyData();
    int GetPktCount();

    bool CheckCRC(char* RawBuffer, int sizeOfBuffer);
    void CalcCRC();
    int Count1s();
    int Count1s(char* rawBuffer, int size);
    char* GenPacket();

    CmdPacket* GetPacket();

    void print();
};