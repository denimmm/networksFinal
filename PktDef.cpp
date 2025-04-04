#include <cstring>

const int FORWARD = 1;
const int BACKWARD = 2;
const int LEFT = 3;
const int RIGHT = 4;
const int HEADERSIZE = 6;
const int BODYSIZE = 3;

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

struct DriveBody{
    char direction;
    char duration;
    char speed;
};

struct CmdPacket{
    Header* header;
    char* data;
    char CRC;
};


class PktDef {


    CmdPacket* cmdPacket;
    char* RawBuffer;
    DriveBody* body;

    PktDef(){
        cmdPacket = new CmdPacket();
        cmdPacket->header = new Header();
        cmdPacket->data = nullptr;
        cmdPacket->CRC = 0;
        RawBuffer = nullptr;
        body = new DriveBody();
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
        memcpy(&cmdPacket->header->Length, incomingPacket + 3, 2);

        //get the body
        body = new DriveBody();
        memcpy(&body->direction, incomingPacket + 5, 1);
        memcpy(&body->duration, incomingPacket + 6, 1);
        memcpy(&body->speed, incomingPacket + 7, 1);
        //get the CRC
        memcpy(&cmdPacket->CRC, incomingPacket + 8, sizeof(cmdPacket->CRC));


        RawBuffer = incomingPacket;
    }

    void SetCmd(CmdType type) {

        switch(type){
            case DRIVE : {
                cmdPacket->header->Drive = 1;
                cmdPacket->header->Sleep = 0;
                cmdPacket->header->Ack = 0;
                return;
            }
            case SLEEP : {
                cmdPacket->header->Sleep = 1;
                cmdPacket->header->Ack = 0;
                cmdPacket->header->Drive = 0;
                return;
            }
            case RESPONSE : {
                cmdPacket->header->Ack = 1;
                cmdPacket->header->Sleep = 0;
                cmdPacket->header->Drive = 0;
                return;
            }
        }

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
        return (cmdPacket->header->Ack == 1);
    }

    //length of the packet in bytes
    int GetLength(){
        return HEADERSIZE + BODYSIZE + sizeof(cmdPacket->CRC);
    }

    char *GetBodyData(){
        return cmdPacket->data;
    }

    int GetPktCount() {
        return cmdPacket->header->PktCount;
    }



    bool CheckCRC(char * RawBuffer, int sizeOfBuffer){
        //get the last point in the buffer
        int i = sizeOfBuffer;
        int CRC = 0;

        //loop through each byte
        for(i; i > 1; i--){
            //get the byte
            char byte = RawBuffer[i];

            while(byte != 0){

                //add one if the last digit is one
                CRC += byte & 1;

                //bitshift right
                byte >> 1;

            }
        }

        return (CRC == cmdPacket->CRC);
    }

    void CalcCRC(){
        //get the last point in the buffer
        int i = sizeof(cmdPacket->data) - 1;
        int CRC = 0;

        //loop through each byte
        for(i; i > 1; i--){
            //get the byte
            char byte = cmdPacket->data[i];

            while(byte != 0){

                //add one if the last digit is one
                CRC += byte & 1;

                //bitshift right
                byte >> 1;

            }
        }
    }

    char *GenPacket(){
        //allocate memory
        RawBuffer = new char[GetLength()];

        //serialize header
        memcpy(RawBuffer, cmdPacket->header, HEADERSIZE);

        //serialize body

        int bodySize = sizeof(cmdPacket->data);
        memcpy(RawBuffer + HEADERSIZE, cmdPacket->data, bodySize);

        //serialize tail
        memcpy(RawBuffer + HEADERSIZE + bodySize, &cmdPacket->CRC, sizeof(cmdPacket->CRC));

    }

};



