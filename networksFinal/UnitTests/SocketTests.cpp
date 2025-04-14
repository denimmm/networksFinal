#include "pch.h"
#include "CppUnitTest.h"
#include "../networksFinal/MySocket.h"
#include "../networksFinal/PktDef.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace MySocketTests
{
    TEST_CLASS(Constructors)
    {
    public:

        TEST_METHOD(ConstructorWithInvalidBuffer)
        {
            MySocket socket(CLIENT, "127.0.0.1", 8080, TCP, 0);  // Invalid buffer size
            Assert::IsTrue(
                socket.GetType() == CLIENT &&
                socket.GetIPAddr().compare("127.0.0.1") == 0&&
                socket.GetPort() == 8080
            );
        }


        TEST_METHOD(ConstructorWithValidBuffer)
        {
            MySocket socket(SERVER, "127.0.0.1", 8080, UDP, 2048);  // Invalid buffer size
            Assert::IsTrue(
                socket.GetType() == SERVER &&
                socket.GetIPAddr().compare("127.0.0.1") == 0 &&
                socket.GetPort() == 8080
            );
        }

    };

    TEST_CLASS(SettersAndGetters)
    {
    public:

        TEST_METHOD(setAndGetIPAddress)
        {
            MySocket socket(CLIENT, "127.0.0.1", 8080, TCP, 0);  // Invalid buffer size

            socket.SetIPAddr("123.456.7.89");

            Assert::IsTrue(
                socket.GetIPAddr().compare("123.456.7.89") == 0
            );
        }
        TEST_METHOD(setAndGetType)
        {
            MySocket socket(CLIENT, "127.0.0.1", 8080, TCP, 0);  // Invalid buffer size

            socket.SetType(SERVER);

            Assert::IsTrue(
                socket.GetType() == SERVER
            );
        }

        TEST_METHOD(setAndGetPort)
        {
            MySocket socket(CLIENT, "127.0.0.1", 8080, TCP, 0);  // Invalid buffer size

            socket.SetPortW(25565);

            Assert::IsTrue(
                socket.GetPort() == 25565
            );
        }

    };
}
