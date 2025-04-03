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

};

struct DriveBody(char* CRC){

    unsigned short DriveBody;

    unsigned int direction : 1;
    unsigned char duration : 10;
    unsigned int speed : 80;
};

struct CmdPacket{
    Header* header;
    char* data;
    char CRC;
};


class PktDef {


    CmdPacket* cmdPacket;
    char* RawBuffer;

    PktDef(){
        cmdPacket = new CmdPacket();
        cmdPacket->header = new Header();
        cmdPacket->data = nullptr;
        cmdPacket->CRC = 0;
        RawBuffer = nullptr;
    }

    //turn raw data into packet object
    PktDef(char * incomingPacket){
        cmdPacket->header = new Header();

        //get the header stuff
        memcpy(&cmdPacket->header->PktCount, incomingPacket, sizeof(cmdPacket->header->PktCount));

        //get the 4 flags
        char flags = incomingPacket[2];
        //remove padding (MIGHT BE BACKWARDS)
        flags >> 4;

        cmdPacket->header->Ack = flags & 1;
        flags >> 1;
        cmdPacket->header->Sleep = flags & 1;
        flags >> 1;
        cmdPacket->header->Status = flags & 1;
        flags >> 1;
        cmdPacket->header->Drive = flags & 1;

        //get the length of the data packet
        memcpy(&cmdPacket->header->Length, incomingPacket + 3, sizeof(cmdPacket->header->Length));

        //get the CRC
        memcpy(&cmdPacket->CRC, incomingPacket + 4, sizeof(cmdPacket->CRC));


        RawBuffer = incomingPacket;
    }

    void SetCmd(CmdType) {
    }

    void SetBodyData(char * bodyData, int dataSize){
        //copy data to the packet's data field
        memcpy(cmdPacket->data, bodyData, dataSize);
    }

    void SetPktCount(int count){
        cmdPacket->header->PktCount = count;

    }

    CmdType GetCmd(){
        if(cmdPacket->header->Drive == 1){
            return DRIVE;
        }
        else if (cmdPacket->header->Sleep == 1){
            return SLEEP;

        }
        else if (cmdPacket->header->Ack == 1){
            return RESPONSE;

        }


    }

    bool GetAck() {



    }

    int GetLength(){

    }

    char *GetBodyData(){




    }

    int GetPktCount() {
        return cmdPacket->header->PktCount;
    }



    bool CheckCRC(char *, int){



    }

    void CalcCRC(){



    }

    char *GenPacket(){




    }




};



