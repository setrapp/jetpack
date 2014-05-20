#pragma once
#ifndef MESSAGE_TYPES
#define MESSAGE_TYPES
class MessageTypes {
public:
	enum class Server
	{
		AddExistingUsers,MovementUpdate, AddNewUser, ReadyUpdate, StartGame
	};
	enum class Client
	{
		Login, MovementUpdate, ReadyUpdate
	};

	enum class MainClass
	{
		PlayerAdd, ReadyUp, StartGame
	};

};
#endif