#pragma once
#ifndef MESSAGE_TYPES
#define MESSAGE_TYPES
class MessageTypes {
public:
	enum class Server
	{
		AddExistingUsers,MovementUpdate, AddNewUser
	};
	enum class Client
	{
		Login, MovementUpdate
	};

	enum class MainClass
	{
		PlayerAdd
	};

};
#endif