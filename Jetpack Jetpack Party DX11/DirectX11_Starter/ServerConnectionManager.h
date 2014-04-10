#pragma once
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <iostream>
#include <process.h>
#include <queue>
#include <string>
#include <map>
#define DEFAULT_PORT "8080"
#define DEFAULT_BUFLEN 512

using namespace std;

#pragma comment(lib, "Ws2_32.lib")
class ServerConnectionManager
{
	
public:
	WSADATA wsaData;
	int iResult;
	ServerConnectionManager(void);
	void initializeServer();
	SOCKET ListenSocket;
	queue<string>* output;
	static void listenForConnections(void* stuff);
	static void manageConnection(void* stuff);
	static void chatThread(void* stuff);
	static std::map <SOCKET, string >ServerConnectionManager::testMap;
	static string printPortData();

};
