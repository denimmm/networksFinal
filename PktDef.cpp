

const int FORWARD = 0;
const int BACKWARD = 0;
const int LEFT = 0;
const int RIGHT = 0;
const int HEADERSIZE = 0;


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

struct DriveBody{



};

class PktDef {

    Header* header;
    char* data;
    char CRC;

    char* RawBuffer;

    PktDef(){
        header = new Header();
        data = nullptr;
        CRC = 0;
    }


    PktDef(char *){



    }

    void SetCmd(CmdType) {



    }

    void SetBodyData(char *, int){



    }

    void SetPktCount(int){



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


    }



    bool CheckCRC(char *, int){



    }

    void CalcCRC(){



    }

    char *GenPacket(){




    }




};



