#include "UDPClientConnectionEntity.h"

int UDPClientConnectionEntity::sock;
queue<string> UDPClientConnectionEntity::networkMessages;
sockaddr_in UDPClientConnectionEntity::echoServAddr; /* Echo server address */ 
sockaddr_in UDPClientConnectionEntity::fromAddr; /* Source address of echo */ 

void UDPClientConnectionEntity::DieWithError(char *errorMessage) 
{ 
 fprintf(stderr,"%s: %d\n", errorMessage, WSAGetLastError()); 
 exit(1); 
} 

UDPClientConnectionEntity::UDPClientConnectionEntity(void)
{
	isConnected=false;
	networkMessages = queue<string>();
}

UDPClientConnectionEntity::~UDPClientConnectionEntity(void)
{
}

void UDPClientConnectionEntity::connectClient(string address){
	isConnected=true;
	if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) /* Load Winsock 2.0 DLL */ 
	 { 
		 fprintf(stderr, "WSAStartup() failed"); 
		 exit(1); 
	 } 

	UDPClientConnectionEntity::sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock < 0){ 
	 DieWithError("socket() failed"); 
	}
	 memset(&echoServAddr, 0, sizeof(echoServAddr)); /* Zero out structure */ 
	 echoServAddr.sin_family = AF_INET; /* Internet address family */ 
	 echoServAddr.sin_addr.s_addr = inet_addr(address.c_str()); /* Server IP address */ 
	 echoServAddr.sin_port = htons(DEFAULT_PORT); /* Server port */ 
	 _beginthread(UDPClientConnectionEntity::listenForResponse,0,(void*)NULL);
	 

}

void UDPClientConnectionEntity::sendMessage(MessageTypes::Client msgType,string sentMessage){

	int curType= (int)msgType;
	string messageTypeString(to_string(curType));
	messageTypeString+= "\n";
	sentMessage= messageTypeString + sentMessage;
	int echoStringLen= sentMessage.length();

	if (sendto(UDPClientConnectionEntity::sock, sentMessage.c_str(), echoStringLen, 0, (struct sockaddr *) 
	&echoServAddr, sizeof(echoServAddr)) != echoStringLen) 
	DieWithError("sendto() sent a different number of bytes than expected"); 
}

void UDPClientConnectionEntity::listenForResponse(void* stuff){
	bool bail=false;
	int respStringLen;
	while(!bail){
		char echoBuffer[ECHOMAX];
		for(int i=0; i<ECHOMAX; i++){
			echoBuffer[i]= '\0';
		}
		 int fromSize = sizeof(fromAddr); 
		 respStringLen = recvfrom(UDPClientConnectionEntity::sock, echoBuffer, ECHOMAX, 0, (struct sockaddr *) (int*)&UDPClientConnectionEntity::fromAddr, (int*)&fromSize);
		 if(respStringLen>0){
			 networkMessages.push(echoBuffer);
		 }
	}
}