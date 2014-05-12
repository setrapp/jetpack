#include "stdafx.h" 
#include <stdio.h> /* for printf(), fprintf() */ 
#include <winsock2.h> /* for socket(),... */ 
#include <stdlib.h> /* for exit() */ 
#pragma comment(lib, "Ws2_32.lib")
#define ECHOMAX 255 /* Longest string to echo */ 
void DieWithError(char *errorMessage) 
{ 
 fprintf(stderr,"%s: %d\n", errorMessage, WSAGetLastError()); 
 exit(1); 
} 
void main(int argc, char *argv[]) 
{ 
 int sock; /* Socket */ 
 struct sockaddr_in echoServAddr; /* Local address */ 
 struct sockaddr_in echoClntAddr; /* Client address */ 
 char echoBuffer[ECHOMAX]; /* Buffer for echo string */ 
 unsigned short echoServPort; /* Server port */ 
 int cliLen; /* Length of incoming message */ 
 int recvMsgSize; /* Size of received message */ 
 WSADATA wsaData; /* Structure for WinSock setup communication */ 


 if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) /* Load Winsock 2.0 DLL */ 
 { 
 fprintf(stderr, "WSAStartup() failed"); 
 exit(1); 
 } 
 /* Create socket for sending/receiving datagrams */ 
 if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) 
 DieWithError("socket() failed");  /* Construct local address structure */ 
 memset(&echoServAddr, 0, sizeof(echoServAddr)); /* Zero out structure */ 
 echoServAddr.sin_family = AF_INET; /* Internet address family */ 
 echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */ 
 echoServAddr.sin_port = htons(8080); /* Local port */ 
 /* Bind to the local address */ 
 if (bind(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0) 
 DieWithError("bind() failed"); 
 
 for (;;) /* Run forever */ 
 { 
 /* Set the size of the in-out parameter */ 
 cliLen = sizeof(echoClntAddr); 
 /* Block until receive message from a client */ 
 if ((recvMsgSize = recvfrom(sock, echoBuffer, ECHOMAX, 0, 
 (struct sockaddr *) &echoClntAddr, &cliLen)) < 0) 
 DieWithError("recvfrom() failed"); 
 printf("Handling client %s\n", inet_ntoa(echoClntAddr.sin_addr)); 
 /* Send received datagram back to the client */ 
 if (sendto(sock, echoBuffer, recvMsgSize, 0, (struct sockaddr *) &echoClntAddr, 
 sizeof(echoClntAddr)) != recvMsgSize) 
 DieWithError("sendto() sent a different number of bytes than expected"); 
 } 
 /* NOT REACHED */ 
} 