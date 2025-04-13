#include "pch.h"
#include "CppUnitTest.h"
#include "../networksFinal/MySocket.h"
#include "../networksFinal/PktDef.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace MySocketTests
{
    TEST_CLASS(MySocketBasicTests)
    {
    public:

        TEST_METHOD(Constructor_InitializesDefaults_WhenInvalidBufferSize)
        {
            MySocket socket(CLIENT, "127.0.0.1", 8080, TCP, 0);  // Invalid buffer size
            Assert::AreEqual(std::string("127.0.0.1"), socket.GetIPAddr());
            Assert::AreEqual(8080, socket.GetPort());
            Assert::AreEqual(CLIENT, socket.GetType());
        }

        TEST_METHOD(Constructor_InitializesCustomSize)
        {
            MySocket socket(CLIENT, "127.0.0.1", 9090, UDP, 2048);
            Assert::AreEqual(9090, socket.GetPort());
        }

        TEST_METHOD(SetAndGetIPAddr)
        {
            MySocket socket(CLIENT, "192.168.1.1", 1234, TCP, 1024);
            socket.SetIPAddr("10.0.0.5");
            Assert::AreEqual(std::string("10.0.0.5"), socket.GetIPAddr());
        }

        TEST_METHOD(SetAndGetPort)
        {
            MySocket socket(SERVER, "127.0.0.1", 1111, UDP, 512);
            socket.SetPort(2222);
            Assert::AreEqual(2222, socket.GetPort());
        }

        TEST_METHOD(SetAndGetSocketType)
        {
            MySocket socket(CLIENT, "127.0.0.1", 3000, TCP, 1024);
            socket.SetType(SERVER);
            Assert::AreEqual(SERVER, socket.GetType());
        }

        TEST_METHOD(GetData_CopiesDataFromBuffer)
        {
            MySocket socket(CLIENT, "127.0.0.1", 8080, TCP, 1024);
            
            char* internalBuffer = reinterpret_cast<char*>(malloc(1024));
            internalBuffer[3] = 10; //Packet size
            for (int i = 0; i < 15; i++) internalBuffer[i] = (char)i;

            memcpy((char*)socket, internalBuffer, 1024);

            char dest[1024] = {};
            int copied = socket.GetData(dest);

            Assert::AreEqual((int)(10 + HEADERSIZE + 1), copied);  //headersize assumed is defined
        }
    };
}
