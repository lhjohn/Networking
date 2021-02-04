
#include <iostream>
#include <lhj_net.h>

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

class CustomServer : public lhj::net::server_interface<CustomMsgTypes>
{
public:
	CustomServer(uint16_t nPort) : lhj::net::server_interface<CustomMsgTypes>(nPort)
	{

	}

protected:
	virtual bool OnClientConnect(std::shared_ptr<lhj::net::connection<CustomMsgTypes>> client)
	{
		lhj::net::message<CustomMsgTypes> msg;
		msg.header.id = CustomMsgTypes::ServerAccept;
		client->Send(msg);
		return true;
	}

	// Called when a client appears to have disconnected
	virtual void OnClientDisconnect(std::shared_ptr<lhj::net::connection<CustomMsgTypes>> client)
	{
		std::cout << "Removing client [" << client->GetID() << "]\n";
	}

	// Called when a message arrives
	virtual void OnMessage(std::shared_ptr<lhj::net::connection<CustomMsgTypes>> client, lhj::net::message<CustomMsgTypes>& msg)
	{
		switch (msg.header.id)
		{
		case CustomMsgTypes::ServerPing:
		{
			std::cout << "[" << client->GetID() << "]: Server Ping\n";

			// Simply bounce message back to client
			client->Send(msg);
		}
		break;

		case CustomMsgTypes::MessageAll:
		{
			std::cout << "[" << client->GetID() << "]: Message All\n";

			// Construct a new message and send it to all clients
			lhj::net::message<CustomMsgTypes> msg;
			msg.header.id = CustomMsgTypes::ServerMessage;
			msg << client->GetID();
			MessageAllClients(msg, client);

		}
		break;

		case CustomMsgTypes::StringMessage:
		{
			std::cout << "[" << client->GetID() << "]: String Message\n";
			size_t stringSize;

			MyFixedString receivedStringStruct;

			msg >> receivedStringStruct;
			
			// msg >> stringSize;
			// char* myValue[6];
			// std::string myValue;

			// for (size_t i = 0; i < 6; i++)
			// {
			// 	/* code */
			// 	msg.body >> myValue[i];
			// }
			
			std::cout << "Value: " << receivedStringStruct.data << std::endl;

			
			// msg >> ;


		}
		break;
		case CustomMsgTypes::ServerDeny:
		case CustomMsgTypes::ServerAccept:
		case CustomMsgTypes::ServerMessage:
		break;
		}
	}
};

int main()
{
	CustomServer server(60000); 
	server.Start();

	while (1)
	{
		server.Update(-1, true);
	}
	
	return 0;
}