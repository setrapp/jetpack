#pragma once
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <iostream>
#include <process.h>
#include <queue>
#define DEFAULT_PORT "8080"
#define DEFAULT_BUFLEN 512

using namespace std;

#pragma comment(lib, "Ws2_32.lib")

class ClientConnectionManager
{
	
	
public:
	WSADATA wsaData;
	int iResult;
	SOCKET ConnectSocket;
	int recvbuflen;
	char recvbuf[DEFAULT_BUFLEN];
	volatile std::queue<char*>* receivedChat;
	ClientConnectionManager(void);
	void sendMessage(string sentMessage);
	void connectClient(string address);
	static void listenForResponse(void* stuff);
	char* stringToChar(string toConvert);
};

