#include <cstring>
#include "PktDef.h"
#include <iostream>

//comment away to remove debug messages
//#define DEBUG

#ifdef DEBUG
    #define DEBUG_MSG(str) std::cout << "[DEBUG] " << str << "\n"
#else
    #define DEBUG_MSG(str)
#endif

using namespace std;

//default constructor.
//sets everything to nullptr or 0
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

    DEBUG_MSG("FLAGS:" << std::hex << (int)flags);

    //find each flag and store it where it belongs
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

    //if the body has data then get the data
    if (cmdPacket->header->Length > 0) {
        //create new char array
        cmdPacket->data = new char[cmdPacket->header->Length];
        //copy data into new char array
        memcpy(cmdPacket->data, incomingPacket + index, cmdPacket->header->Length);
        index += cmdPacket->header->Length;

    }

    //get the CRC
    memcpy(&cmdPacket->CRC, incomingPacket + index, sizeof(cmdPacket->CRC));

    //copy the entire buffer into RawBuffer
    RawBuffer = new char[GetLength()];
    memcpy(RawBuffer, incomingPacket, GetLength());
}

//sets the command flag of the packet header
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

//returns the command flag of the packet header
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
    else if (cmdPacket->header->Status == 1) {
        return STATUS;
    }
    else {
        return UNKOWN;

    }
}

//set the body of the packet. use ((char*)body, sizeof(*body)) to implement
void PktDef::SetBodyData(char* bodyData, int dataSize) {
    //initialize new cmdpacket if required
    if (cmdPacket == nullptr) {
        cmdPacket = new CmdPacket;
    }
    //make new char array
    cmdPacket->data = new char[dataSize];
    //copy data into new array
    memcpy(cmdPacket->data, bodyData, dataSize);

    //update size
    cmdPacket->header->Length = dataSize;
}

//returns the body of the packet
char* PktDef::GetBodyData() {
    return cmdPacket->data;
}

//sets the packet count to the parameter
void PktDef::SetPktCount(int count) {
    cmdPacket->header->PktCount = abs(count);
}

//returns the packet's current packet coutn
int PktDef::GetPktCount() {
    return cmdPacket->header->PktCount;
}

//returns true or false based on if the packet is an ack packet or not.
bool PktDef::GetAck() {
    return (cmdPacket->header->Ack == 1);
}

//returns the length of the entire packet including header, body, and crc
int PktDef::GetLength() {
    int size = HEADERSIZE + cmdPacket->header->Length + sizeof(cmdPacket->CRC);
    DEBUG_MSG("GetLength() = " << size);
    return size;
}

//calculate the buffer's crc and compare it to the actual crc of the buffer
//returns bool based on whether the crc is correct or not
bool PktDef::CheckCRC(char* RawBuffer, int sizeOfBuffer) {
    //initialize expectedCRC
    unsigned char ExpectedCRC = 0;
    //get the index of the byte containing crc
    int CRCIndex = sizeOfBuffer - 1;
    //copy thr crc byte into the expectedCRC char
    memcpy(&ExpectedCRC, RawBuffer + CRCIndex, 1);

    //calculates the actual crc of the buffer
    int ActualCRC = Count1s(RawBuffer, sizeOfBuffer);

    //compare and return
    return (ExpectedCRC == ActualCRC);
}

//calculates and sets the crc of the packet
void PktDef::CalcCRC() {
    //set the crc
    cmdPacket->CRC = Count1s();
    DEBUG_MSG("set crc: " << cmdPacket->CRC);
}


//counts the total # of 1s in the data for the buffer in binary
//does not include the crc in the count
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

//counts the total # of 1s in the data for this packet in binary
//does not include the crc in the count
int PktDef::Count1s() {

    //get crc of header
    int CRC = 0;
    int i = 0;

    //for each byte, extract 1s until it is equal to zero
    for (i; i < HEADERSIZE; i++) {
        DEBUG_MSG("current i: " << i);
        char byte = ((char*)cmdPacket->header)[i];

        //count 1s in this byte
        while (byte != 0) {
            CRC += byte & 1;
            byte >>= 1;
        }
    }
    

    //get crc of the data
        for (i = 0; i < cmdPacket->header->Length; i++) {
            DEBUG_MSG("current i: " << i);
            char byte = cmdPacket->data[i];

            //count 1s in this byte
            while (byte != 0) {
                CRC += byte & 1;
                byte >>= 1;
            }
        }

    return CRC;
}

//creates the rawdata for this packet and assigns it to its rawdata attribute
//returns a reference to the rawdata attribute
char* PktDef::GenPacket() {

    //get the length of the packet
    int length = GetLength();
    //determine whether the packet has data or not (ack packets contain no data)
    bool hasData = (cmdPacket->data != nullptr);

    DEBUG_MSG("generating packet:\n");
    DEBUG_MSG("size: " << length);

    //delete the old rawbuffer if this has been done before (it shouldnt have)
    if (RawBuffer != nullptr) {
        delete RawBuffer;
    }

    //5 is the smallest possible packet. sanity check because im going insane
    if (length < HEADERSIZE + 1) {
        length = HEADERSIZE + 1;
    }

    //initialize a new buffer
    RawBuffer = new char[length];



    //copy header to rawpacket
    memcpy(RawBuffer, cmdPacket->header, HEADERSIZE);

    //debug messages
    DEBUG_MSG("RAWBUFFER: " << RawBuffer);
    if (hasData) DEBUG_MSG("DATA: " << cmdPacket->data);
    DEBUG_MSG("DATALENGTH: " << cmdPacket->header->Length);
    
    //copy the data to the rawBuffer if the packet has data
    if (hasData) memcpy(RawBuffer + HEADERSIZE, cmdPacket->data, cmdPacket->header->Length);

    //copy the crc int the buffer
    memcpy(RawBuffer + HEADERSIZE + cmdPacket->header->Length, &cmdPacket->CRC, sizeof(cmdPacket->CRC));

    //return the buffer
    return RawBuffer;

}

//returns a reference to the cmdpacket for testing.
CmdPacket* PktDef::GetPacket() {
    return cmdPacket;
}

//prints the packet's information for testing.
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



