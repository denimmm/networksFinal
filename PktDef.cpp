#include <cstring>

const int FORWARD = 0;
const int BACKWARD = 0;
const int LEFT = 0;
const int RIGHT = 0;
const int HEADERSIZE = 0;

using namespace std;

enum CmdType{

DRIVE = 0, SLEEP = 1, RESPONSE = 2

};

struct Header{

    unsigned short PktCount;

    unsigned int Drive : 1;
    unsigned int Status : 1;
    unsigned int Sleep : 1;
    unsigned int Ack : 1;

    unsigned int Padding : 4;

    unsigned short Length;
    
    char CRC;

};

struct DriveBody(char* CRC){

unsigned short DriveBody;

unsigned int direction : 1;
unsigned char duration : 10;
unsigned int speed : 80;
};

struct CmdPacket{
 Header header;
 char* data;
 char CRC;
}

CmdPacket packet;

char* RawBuffer;

class PktDef {

    Header* header;
    char* data;
    char CRC;

    char* RawBuffer;

    PktDef(){
        header = new Header();
        data = nullptr;
        CRC = 0;
        RawBuffer = nullptr;
    }


    //turn raw data into packet object
    PktDef(char * incomingPacket){
        header = new Header();

        //get the header stuff
        memcpy(&header->PktCount, incomingPacket, sizeof(header->PktCount));

        //get the 4 flags
        char flags = incomingPacket[2];
        //remove padding (MIGHT BE BACKWARDS)
        flags >> 4;

        header->Ack = flags & 1;
        flags >> 1;
        header->Sleep = flags & 1;
        flags >> 1;
        header->Status = flags & 1;
        flags >> 1;
        header->Drive = flags & 1;

        //get the length of the data packet
        memcpy(&header->Length, incomingPacket + 3, sizeof(header->Length));

        //get the CRC
        memcpy(&header->CRC, incomingPacket + 4, sizeof(header->CRC));
        CRC = header->CRC;

        RawBuffer = incomingPacket;
    }

    void SetCmd(CmdType) {



    }

    void SetBodyData(char *, int){



    }

    void SetPktCount(int count){
    packet.header.PktCount = count;
    }

    CmdType GetCmd(){



    }

    bool GetAck() {



    }

    int GetLength(){

    }

    char *GetBodyData(){




    }

    int GetPktCount() {
    return packet.header.PktCount;
    }



    bool CheckCRC(char *, int){



    }

    void CalcCRC(){



    }

    char *GenPacket(){




    }




};



