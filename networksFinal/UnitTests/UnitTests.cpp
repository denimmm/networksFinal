#include "pch.h"
#include "CppUnitTest.h"
#include "../networksFinal/PktDef.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest
{
	TEST_CLASS(setters)
	{
	public:

		TEST_METHOD(setCmdDRIVE)
		{
			//arrange
			PktDef* packet = new PktDef();
			//act
			packet->SetCmd(DRIVE);

			//assert
			Assert::IsTrue(packet->GetCmd() == DRIVE);//AreEqual doesnt work.

			delete packet;
		}
		TEST_METHOD(setCmdSLEEP)
		{
			//arrange
			PktDef* packet = new PktDef();
			//act
			packet->SetCmd(SLEEP);

			//assert
			Assert::IsTrue(packet->GetCmd() == SLEEP);

			delete packet;
		}
		TEST_METHOD(setCmdRESPONSE)
		{
			//arrange
			PktDef* packet = new PktDef();
			//act
			packet->SetCmd(RESPONSE);

			//assert
			Assert::IsTrue(packet->GetCmd() == RESPONSE);

			delete packet;
		}

		TEST_METHOD(setBodyDataDrive)
		{
			//arrange
			PktDef* packet = new PktDef();

			DriveBody* body = new DriveBody;

			body->direction = 1;
			body->duration = 20;
			body->speed = 100;

			//act
			packet->SetBodyData((char*)body, sizeof(*body));

			//assert
			DriveBody* asdf = new DriveBody;
			memcpy(asdf, packet->GetBodyData(), sizeof(*asdf));

			bool success = (asdf->direction == 1 && asdf->duration == 20 && asdf->speed == 100);


			Assert::IsTrue(success);

			delete packet;
			delete asdf;
		}

		TEST_METHOD(setBodyDataTelemetry)
		{
			//arrange
			PktDef* packet = new PktDef();

			StatusBody* body = new StatusBody;

			body->CurrentGrade = 100;
			body->HitCount = 0;
			body->LastCmd = 4;
			body->LastCmdSpeed = 100;
			body->LastCmdValue = 20;
			body->LastPktCounter = 14;

			//act
			packet->SetBodyData((char*)body, sizeof(*body));

			//assert
			StatusBody* asdf = new StatusBody;
			memcpy(asdf, packet->GetBodyData(), sizeof(*asdf));

			bool success = (asdf->CurrentGrade == 100 &&
				asdf->HitCount == 0 &&
				asdf->LastCmd == 4 &&
				asdf->LastCmdSpeed == 100 &&
				asdf->LastCmdValue == 20 &&
				asdf->LastPktCounter == 14
				);

			Assert::IsTrue(success);

			delete packet;
			delete asdf;
		}

		TEST_METHOD(setPktCount)
		{
			//arrange
			PktDef* packet = new PktDef();
			//act
			packet->SetPktCount(12);

			//assert
			Assert::IsTrue(packet->GetPktCount() == 12);

			delete packet;
		}


	};

	TEST_CLASS(getters) {

	public:
		TEST_METHOD(GetAckTrue) {
			//arrange
			PktDef* packet = new PktDef();
			//act
			packet->SetCmd(RESPONSE);
			

			//assert
			Assert::IsTrue(packet->GetAck() == true);

			delete packet;
		}
		TEST_METHOD(GetAckFalse) {
			//arrange
			PktDef* packet = new PktDef();
			//act
			packet->SetCmd(DRIVE);

			//assert
			Assert::IsTrue(packet->GetAck() == false);

			delete packet;
		}

		TEST_METHOD(GetLength) {
			//arrange
			PktDef* packet = new PktDef();
			DriveBody* body = new DriveBody;
			body->direction = 1;
			body->duration = 10;
			body->speed = 100;

			//act
			packet->SetCmd(DRIVE);
			packet->SetBodyData((char*)body, 3);

			packet->CalcCRC();
			//assert
			Assert::AreEqual(packet->GetLength(), 8);

			delete packet;

		}

	};


	TEST_CLASS(constructors) 
	{
	public:
		TEST_METHOD(defaultConstructorTest)
		{
			//act
			PktDef pktdef;
			CmdPacket* cmdPacket = pktdef.GetPacket();

			//assert
			Assert::IsTrue(
				cmdPacket->header->Ack == 0 &&
				cmdPacket->header->Drive == 0 &&
				cmdPacket->header->Length == 0 &&
				cmdPacket->header->Padding == 0 &&
				cmdPacket->header->PktCount == 0 &&
				cmdPacket->header->Sleep == 0 &&
				cmdPacket->header->Status == 0 &&

				cmdPacket->data == nullptr &&

				cmdPacket->CRC == 0 &&

				pktdef.GetBodyData() == nullptr
			);

		}

		TEST_METHOD(RawDataConstructorTestDriveBody)
		{
			//arrange
			PktDef pkt1;
			pkt1.SetCmd(DRIVE);

			DriveBody* body = new DriveBody;
			body->direction = 1;
			body->duration = 10;
			body->speed = 100;
			pkt1.SetBodyData((char*)body, sizeof(*body));

			pkt1.CalcCRC();

			char* string = pkt1.GenPacket();


			// arrange
			PktDef pkt2(string);
			CmdPacket* cmdPacket = pkt2.GetPacket();
			Assert::IsTrue(
				//(int)cmdPacket->header->PktCount == 0 &&
				
				//cmdPacket->header->Drive == 1 &&
				//cmdPacket->header->Sleep == 0 &&
				//cmdPacket->header->Ack == 0 &&
				//cmdPacket->header->Status == 0 &&
				
				//cmdPacket->header->Padding == 0 &&

				//(int)cmdPacket->header->Length == 3 &&

				//strcmp(pkt2.GetBodyData(), (char*)body) &&

				(int)cmdPacket->CRC == 9
			);

		}
	};



	TEST_CLASS(CRC) {

	public:
		TEST_METHOD(calculateCRC)
		{

			//arrange
			PktDef* packet = new PktDef();

			packet->SetCmd(DRIVE); // +1

			packet->SetPktCount(1); // +1

			DriveBody* body = new DriveBody;
			body->direction = 1; // +1
			body->duration = 10; // +2
			body->speed = 100; // +3

			packet->SetBodyData((char*)body, 3);
			// +2 for length bc body is 3 long


			//act
			packet->CalcCRC();

			//assert
			Assert::AreEqual(packet->GetPacket()->CRC, (unsigned char)10);

			delete packet;
			delete body;
		}
		TEST_METHOD(checkCRC)
		{
			//arrange
			PktDef* packet = new PktDef();
			DriveBody* body = new DriveBody;
			body->direction = 1; // +1
			body->duration = 10; // +2
			body->speed = 100; // +3

			packet->SetBodyData((char*)body, 3);

			packet->SetCmd(DRIVE); // +1

			packet->SetPktCount(1); // +1
			//act
			packet->CalcCRC();

			char* string = packet->GenPacket();

			//assert
			Assert::IsTrue(packet->CheckCRC(string, packet->GetLength()));

			delete packet;

		}



	};


	TEST_CLASS(PacketGeneration) {

	public:


	};
}
