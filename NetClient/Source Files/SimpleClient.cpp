#include <iostream>
#include <lhj_net.h>
#include <unistd.h>
#include <thread>
#include <future>


enum class CustomMsgTypes : uint32_t
{
	ServerAccept,
	ServerDeny,
	ServerPing,
	MessageAll,
	ServerMessage,
	StringMessage
};

struct MyFixedString
{
  char data[20];
};

class CustomClient : public lhj::net::client_interface<CustomMsgTypes>
{
public:
	void PingServer()	
	{
		lhj::net::message<CustomMsgTypes> msg;
		msg.header.id = CustomMsgTypes::ServerPing;

		// Caution with this...
		std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();		

		msg << timeNow;
		Send(msg);
	}

	void MessageAll()
	{
		lhj::net::message<CustomMsgTypes> msg;
		msg.header.id = CustomMsgTypes::MessageAll;		
		Send(msg);
	}

	void StringMessage(MyFixedString myString) {
		lhj::net::message<CustomMsgTypes> msg;
		msg.header.id = CustomMsgTypes::StringMessage;

		msg << myString;
		// msg << strlen(myValue);
		// const char* myValue = "example";
		// for (size_t i = 0; i < strlen(myValue); i++)
		// {
		// 	/* code */
		// 	msg << *myValue + i;
		// }
		Send(msg);
	}
};

CustomClient c;
uint64_t i = 1;

// The function we want to execute on the new thread.
// void task_t()
// {
// 	while(true) {
// 		if(i%200000000==0) {
// 			c.StringMessage("Henrik");
// 			std::cout << "Connected: " << c.IsConnected() << std::endl;
// 			// usleep(1000000);
// 		}
// 		i++;
// 	}
// }

int main()
{
	c.Connect("127.0.0.1", 60000);

	// bool key[3] = { false, false, false };
	// bool old_key[3] = { false, false, false };
	// if (c.IsConnected()) {
			// auto a1 = std::async(std::launch::async, task_t);
	// }

	MyFixedString aString;
	//aString->data = "Henrik";
	strncpy(aString.data, "Henrik in da house", 20);

	bool bQuit = false;
	while (!bQuit)
	{
		// if (GetForegroundWindow() == GetConsoleWindow())
		// {
		// 	key[0] = GetAsyncKeyState('1') & 0x8000;
		// 	key[1] = GetAsyncKeyState('2') & 0x8000;
		// 	key[2] = GetAsyncKeyState('3') & 0x8000;
		// }
		// c.MessageAll();
		//c.MessageAll();
		// usleep(100);

		// if (key[0] && !old_key[0]) c.PingServer();
		// if (key[1] && !old_key[1]) c.MessageAll();
		// if (key[2] && !old_key[2]) bQuit = true;

		// for (int i = 0; i < 3; i++) old_key[i] = key[i];

		if (c.IsConnected())
		{
			if(i%100000000==0) {
				c.StringMessage(aString);
				std::cout << "Sending a string" << std::endl;
				// usleep(1000000);
			}
			i++;
			if (!c.Incoming().empty())
			{
				auto msg = c.Incoming().pop_front().msg;

				switch (msg.header.id)
				{
				case CustomMsgTypes::ServerAccept:
				{
					// Server has responded to a ping request				
					std::cout << "Server Accepted Connection\n";
				}
				break;

				case CustomMsgTypes::ServerPing:
				{
					// Server has responded to a ping request
					std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();
					std::chrono::system_clock::time_point timeThen;
					msg >> timeThen;
					std::cout << "Ping: " << std::chrono::duration<double>(timeNow - timeThen).count() << "\n";
				}
				break;

				case CustomMsgTypes::ServerMessage:
				{
					// Server has responded to a ping request	
					uint32_t clientID;
					msg >> clientID;
					std::cout << "Hello from [" << clientID << "]\n";
				}
				break;

				case CustomMsgTypes::StringMessage:
				{

				}
				break;
				case CustomMsgTypes::MessageAll:
				case CustomMsgTypes::ServerDeny:
				break;
				}
			}
		}
		else
		{
			std::cout << "Server Down\n";
			bQuit = true;
		}

	}

	return 0;
}