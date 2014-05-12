#pragma once
#define WIN32_LEAN_AND_MEAN
#include "stdafx.h"
#include <stdio.h> /* for printf(), fprintf() */ 
#include <winsock2.h> /* for socket(),... */ 
#include <stdlib.h> /* for exit() */ 
#include <process.h>
#include <queue>
#define DEFAULT_PORT 8080
#define ECHOMAX 255 /* Longest string to echo */ 
using namespace std;
class UDPClientConnectionEntity
{
public:
	WSADATA wsaData; /* Structure for WinSock setup communication */ 
	static int sock; /* Socket descriptor */ 
	static struct sockaddr_in echoServAddr; /* Echo server address */ 
	static struct sockaddr_in fromAddr; /* Source address of echo */ 
	unsigned short echoServPort; /* Echo server port */ 
	unsigned int fromSize; /* In-out of address size for recvfrom() */ 
	char *servIP; /* IP address of server */ 
	char *echoString; /* String to send to echo server */ 
	bool isConnected;
	void DieWithError(char* errorMessage);
	void sendMessage(string sentMessage);
	void connectClient(string address);
	static void listenForResponse(void* stuff);
	static queue<string> networkMessages;
	UDPClientConnectionEntity(void);
	~UDPClientConnectionEntity(void);
};

