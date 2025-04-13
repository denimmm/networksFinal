#include "pch.h"
#include "CppUnitTest.h"
#include "../networksFinal/MySocket.h"
#include "../networksFinal/PktDef.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SocketTests
{
	TEST_CLASS(settersAndGetters)
	{
	public:
		TEST_METHOD(setType)
		{
			MySocket* socket = new MySocket();
			

			delete socket;
		}



	};
}