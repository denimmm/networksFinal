#include <cstring>
#include "PktDef.h"
#include <iostream>

#define DEBUG//comment away to remove debug messages

#ifdef DEBUG
    #define DEBUG_MSG(str) std::cout << "[DEBUG] " << str << "\n"
#else
    #define DEBUG_MSG(str)
#endif

using namespace std;

PktDef::PktDef(void) {
    cmdPacket = new CmdPacket();
    cmdPacket->header = new Header();
    cmdPacket->data = nullptr;
    cmdPacket->CRC = 0;
    RawBuffer = nullptr;
}

//turn raw data into packet object
PktDef::PktDef(char* incomingPacket) {

    //intialize structs
    cmdPacket = new CmdPacket;
    cmdPacket->header = new Header();

    //make index for memcpy from the buffer
    int index = 0;

    //copy the PktCount from the buffy
    memcpy(&cmdPacket->header->PktCount, incomingPacket, 2);
    index += 2;

    //get the 4 flags

    uint8_t flags;
    memcpy(&flags, incomingPacket + index, 1);
    index += 1;

    //remove padding (MIGHT BE BACKWARDS)
    DEBUG_MSG("FLAGS:" << std::hex << (int)flags);

    //flags >>= 4;

    cmdPacket->header->Drive = flags & 1;
    flags >>= 1;
    cmdPacket->header->Status = flags & 1;
    flags >>= 1;
    cmdPacket->header->Sleep = flags & 1;
    flags >>= 1;
    cmdPacket->header->Ack = flags & 1;







    //get the length of the data packet
    memcpy(&cmdPacket->header->Length, incomingPacket + index, 1);
    index += 1;

    //if the body has stuff then get the stuff
    if (cmdPacket->header->Length > 0) {
        cmdPacket->data = new char[cmdPacket->header->Length];
        memcpy(cmdPacket->data, incomingPacket + index, cmdPacket->header->Length);
        index += cmdPacket->header->Length;

    }

    //get the CRC
    memcpy(&cmdPacket->CRC, incomingPacket + index, sizeof(cmdPacket->CRC));

    RawBuffer = new char[GetLength()];
    memcpy(RawBuffer, incomingPacket, GetLength());
}

void PktDef::SetCmd(CmdType type) {
    switch (type) {
        case DRIVE: {
            cmdPacket->header->Drive = 1;
            cmdPacket->header->Sleep = 0;
            cmdPacket->header->Ack = 0;
            cmdPacket->header->Status = 0;
            return;
        }
        case SLEEP: {
            cmdPacket->header->Sleep = 1;
            cmdPacket->header->Ack = 0;
            cmdPacket->header->Drive = 0;
            cmdPacket->header->Status = 0;
            return;
        }
        case RESPONSE: {
            cmdPacket->header->Ack = 1;
            cmdPacket->header->Sleep = 0;
            cmdPacket->header->Drive = 0;
            cmdPacket->header->Status = 0;
            return;
        }
        case STATUS: {
            cmdPacket->header->Status = 1;
            cmdPacket->header->Sleep = 0;
            cmdPacket->header->Drive = 0;
            cmdPacket->header->Ack = 0;
            return;
        }
    }
}

CmdType PktDef::GetCmd() {
    if (cmdPacket->header->Drive == 1) {
        return DRIVE;
    }
    else if (cmdPacket->header->Sleep == 1) {
        return SLEEP;
    }
    else if (cmdPacket->header->Ack == 1) {
        return RESPONSE;
    }
    else {
        return UNKOWN;

    }
}

void PktDef::SetBodyData(char* bodyData, int dataSize) {
    if (cmdPacket == nullptr) {
        cmdPacket = new CmdPacket;
    }

    cmdPacket->data = new char[dataSize];
    memcpy(cmdPacket->data, bodyData, dataSize);

    //update size
    cmdPacket->header->Length = dataSize;
}

char* PktDef::GetBodyData() {
    return cmdPacket->data;
}

void PktDef::SetPktCount(int count) {
    cmdPacket->header->PktCount = count;
}

int PktDef::GetPktCount() {
    return cmdPacket->header->PktCount;
}

bool PktDef::GetAck() {
    return (cmdPacket->header->Ack == 1);
}

int PktDef::GetLength() {
    int size = HEADERSIZE + cmdPacket->header->Length + sizeof(cmdPacket->CRC);
    DEBUG_MSG("GetLength() = " << size);
    return size;
}

//calculate the buffer's crc and compare it to the actual crc of the buffer
bool PktDef::CheckCRC(char* RawBuffer, int sizeOfBuffer) {
    unsigned char ExpectedCRC = 0;
    int CRCIndex = sizeOfBuffer - 1; //might be minus 2
    memcpy(&ExpectedCRC, RawBuffer + CRCIndex, 1);

    int ActualCRC = Count1s(RawBuffer, sizeOfBuffer);

    return (ExpectedCRC == ActualCRC);
}

void PktDef::CalcCRC() {
    //set the crc
    cmdPacket->CRC = Count1s();
    DEBUG_MSG("set crc: " << cmdPacket->CRC);
}



int PktDef::Count1s(char* RawBuffer, int size) {

    //get crc of header
    int CRC = 0;   

    for (int x = 0; x < size -1; x++) {
        char byte = *(RawBuffer + x);

        while (byte != 0) {
            CRC += byte & 1;
            byte >>= 1;
        }
    }

    return CRC;
}

int PktDef::Count1s() {

    //get crc of header
    int CRC = 0;
    int i = 0;

    //for each byte, extract 1s until it is equal to zero
    for (i; i < HEADERSIZE; i++) {
        DEBUG_MSG("current i: " << i);
        char byte = ((char*)cmdPacket->header)[i];

        while (byte != 0) {
            CRC += byte & 1;
            byte >>= 1;
        }
    }
    

    //get crc of the data

        for (i = 0; i < cmdPacket->header->Length; i++) {
            DEBUG_MSG("current i: " << i);
            char byte = cmdPacket->data[i];

            while (byte != 0) {
                CRC += byte & 1;
                byte >>= 1;
            }
        }

    return CRC;
}

char* PktDef::GenPacket() {
    int length = GetLength();
    bool hasData = (cmdPacket->data != nullptr);

    DEBUG_MSG("generating packet:\n");
    DEBUG_MSG("size: " << length);

    if (RawBuffer != nullptr) {
        delete RawBuffer;
    }

    //5 is the smallest possible packet
    if (length < HEADERSIZE + 1) {
        length = HEADERSIZE + 1;
    }

    RawBuffer = new char[length];



    //copy header to rawpacket
    memcpy(RawBuffer, cmdPacket->header, HEADERSIZE);

    DEBUG_MSG("RAWBUFFER: " << RawBuffer);
    if (hasData) DEBUG_MSG("DATA: " << cmdPacket->data);

    DEBUG_MSG("DATALENGTH: " << cmdPacket->header->Length);
    
    //copy the data to the rawBuffer
    if (hasData) memcpy(RawBuffer + HEADERSIZE, cmdPacket->data, cmdPacket->header->Length);

    memcpy(RawBuffer + HEADERSIZE + cmdPacket->header->Length, &cmdPacket->CRC, sizeof(cmdPacket->CRC));

    return RawBuffer;

}


CmdPacket* PktDef::GetPacket() {
    return cmdPacket;
}

void PktDef::print() {
    DEBUG_MSG("COMMAND PACKET : \n"
        << " - HEADER:\n"
        << " - - PktCount: " << cmdPacket->header->PktCount << "\n"
        << " - - Drive: " << cmdPacket->header->Drive << "\n"
        << " - - Status: " << cmdPacket->header->Status << "\n"
        << " - - Sleep: " << cmdPacket->header->Sleep << "\n"
        << " - - Ack: " << cmdPacket->header->Ack << "\n"
        << " - - Padding: " << cmdPacket->header->Padding << "\n"
        << " - - Length: " << (int)cmdPacket->header->Length << "\n"
        << " - Data: " << cmdPacket->data << "\n"
        << " - CRC: " << (int)cmdPacket->CRC << "\n"
        );
}



