// networksFinal.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "PktDef.h"

#include <iostream>

using namespace std;

void printAsHex(char* string, int size) {

        for (int i = 0; i < size; i++) {

            cout << i << ": " << hex << (int)string[i] << "\n";
        }
        cout << "\n";
    
}

int main()
{
    PktDef* packet = new PktDef();

    packet->SetCmd(DRIVE);

    packet->SetPktCount(1);

    DriveBody* body = new DriveBody;
    body->direction = 1;
    body->duration = 10;
    body->speed = 100;

    packet->SetBodyData((char*)body, sizeof(*body));

    packet->CalcCRC();

    char * rawData = packet->GenPacket();


    packet->print();

    PktDef* packet2 = new PktDef(rawData);

    //packet2->print();

    cout << "\nPACKET 1 RAWDATA: \n";
    printAsHex(rawData, packet->GetLength());

    cout << "PACKET 2 RAWDATA: \n";
    printAsHex(packet2->GenPacket(), packet2->GetLength());

    delete packet;
    delete packet2;
    delete body;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
