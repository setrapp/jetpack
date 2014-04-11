#include "ClientConnectionEntity.h"

int ClientConnectionEntity::socketNum;
ClientConnectionEntity::ClientConnectionEntity(void)
{
	receivedChat = new std::queue<char*>();
	connectClient("127.0.0.1");
}


void ClientConnectionEntity::sendMessage(string sentMessage){
	
	char *sendbuf = stringToChar(sentMessage);

	int iResult;

	// Send an initial buffer
	//iResult = send(ConnectSocket, sendbuf, (int) strlen(sendbuf), 0);
	iResult = send(ConnectSocket, sendbuf, 512, 0);
	if (iResult == SOCKET_ERROR) {
		printf("send failed: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
	}

	//printf("Bytes Sent: %ld\n", iResult);

	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
	}
	

}

void ClientConnectionEntity::connectClient(string address){
	iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed: %d\n", iResult);
	}

	struct addrinfo *result = NULL,
            *ptr = NULL,
            hints;

	ZeroMemory( &hints, sizeof(hints) );
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	bool connectionFound=false;

	iResult = getaddrinfo(stringToChar(address), DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed: %d\n", iResult);
		WSACleanup();
	}

	ConnectSocket= INVALID_SOCKET;
	
	// Attempt to connect to the first address returned by
	// the call to getaddrinfo
	ptr=result;

	// Create a SOCKET for connecting to server
	ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, 
		ptr->ai_protocol);

	if (ConnectSocket == INVALID_SOCKET) {
		cout<<"Error at socket()";
		freeaddrinfo(result);
		WSACleanup();
	}

	// Connect to server.
	iResult = connect( ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
	// closesocket(ConnectSocket);
	int fdfs= WSAGetLastError();
	cout<<"Connect doesn't work";
	// ConnectSocket = INVALID_SOCKET;
	}

	// Should really try the next address returned by getaddrinfo
	// if the connect call failed
	// But for this simple example we just free the resources
	// returned by getaddrinfo and print an error message

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) {
		cout <<"Unable to connect to server!\n";
		WSACleanup();
	}	
	_beginthread(ClientConnectionEntity::listenForResponse,0,(void*)ConnectSocket);


}

char* ClientConnectionEntity::stringToChar(string stringToConvert){
	const char* s= stringToConvert.c_str();
	return const_cast<char *>(s);
}

void ClientConnectionEntity::listenForResponse(void* stuff){
	bool bail= false;
	SOCKET connectedSocket= (SOCKET)stuff;
	int buflen= DEFAULT_BUFLEN;
	int iResult= 0;
	char* receiveBuffer= new char[DEFAULT_BUFLEN];
	string s= "SOCKET REQUEST";
	send(connectedSocket, s.c_str(), 512, 0);
	int counter=0;
	do {
		for(int i=0; i<DEFAULT_BUFLEN; i++){
			receiveBuffer[i]= '\0';
		}
		iResult = recv(connectedSocket, receiveBuffer, buflen, 0);
    if (iResult > 0){
		if(counter==0){
			ClientConnectionEntity::socketNum=atoi (receiveBuffer);
			cout << "MY SOCKET NUMBER IS: " << ClientConnectionEntity::socketNum << '\n';
		}
		else{

			cout << "client received: " << (char*)receiveBuffer << '\n';
		}
		counter++;
	}    
    else if (iResult == 0){
        printf("Connection closed?\n");
		bail=true;
	}
    else{
		int fdfs= WSAGetLastError();

		bail=true;
	}
	} while (!bail);
	printf("CLIENT LOOP HAS ENDED");
}