#include "stdafx.h"
#include <stdio.h> /* for printf(), fprintf() */ 
#include <winsock2.h> /* for socket(),... */ 
#include <stdlib.h> /* for exit() */ 
#include <iostream>
#include <string>
#include "UDPClientConnectionEntity.h"
#pragma comment(lib, "Ws2_32.lib")
#define ECHOMAX 255 /* Longest string to echo */ 
using namespace std;
static UDPClientConnectionEntity uc;
int _tmain(int argc, _TCHAR* argv[])
{
	uc= UDPClientConnectionEntity();
	uc.connectClient("127.0.0.1");
	while(true){
		char input[500];
		cin>> input;
		uc.sendMessage(input);
	}
}





