#pragma once
#define WIN32_LEAN_AND_MEAN
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <iostream>
#include <process.h>
#include <queue>
#include <sstream>
#include "Entity.h"
#include "MessageTypes.h"
#define DEFAULT_PORT "8080"
#define DEFAULT_BUFLEN 512

using namespace std;

#pragma comment(lib, "Ws2_32.lib")

class ClientConnectionEntity: public Entity
{
	
	
public:
	WSADATA wsaData;
	int iResult;
	SOCKET ConnectSocket;
	int recvbuflen;
	static int socketNum;
	char recvbuf[DEFAULT_BUFLEN];
	volatile std::queue<char*>* receivedChat;
	ClientConnectionEntity(void);
	~ClientConnectionEntity();
	void sendMessage(MessageTypes::Client msgType, string sentMessage);
	void connectClient(string address);
	static void listenForResponse(void* stuff);
	static queue<string> networkMessages;
	bool isConnected;
};