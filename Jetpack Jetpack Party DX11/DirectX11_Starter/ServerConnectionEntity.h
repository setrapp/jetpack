#pragma once
#define WIN32_LEAN_AND_MEAN
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <iostream>
#include <process.h>
#include <queue>
#include <string>
#include <map>
#include "Entity.h"
#define DEFAULT_PORT "8080"

#define DEFAULT_BUFLEN 512

using namespace std;
#pragma comment(lib, "Ws2_32.lib")
class ServerConnectionEntity: public Entity
{

public:
	WSADATA wsaData;
	int iResult;
	ServerConnectionEntity(void);
	void initializeServer();
	SOCKET ListenSocket;
	queue<string>* output;
	static void listenForConnections(void* stuff);
	static void manageConnection(void* stuff);
	static void chatThread(void* stuff);
	static std::map <SOCKET, string >ServerConnectionEntity::testMap;
	static string printPortData();

};