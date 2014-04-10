#include "ClientConnectionManager.h"


ClientConnectionManager::ConnectionManager(void)
{
	receivedChat = new std::queue<char*>();
}


void ClientConnectionManager::sendMessage(String^ sentMessage){
	recvbuflen = DEFAULT_BUFLEN;

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

void ClientConnectionManager::connectClient(String^ address){
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
    closesocket(ConnectSocket);
	cout<<"Connect doesn't work";
    ConnectSocket = INVALID_SOCKET;
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
	_beginthread(ConnectionManager::listenForResponse,0,(void*)ConnectSocket);


}

char* ClientConnectionManager::stringToChar(String^ stringToConvert){
	IntPtr ptrToNativeString = Marshal::StringToHGlobalAnsi(stringToConvert);
	char* nativeString = static_cast<char*>(ptrToNativeString.ToPointer());
	return nativeString;
}

void ClientConnectionManager::listenForResponse(void* stuff){
	bool bail= false;
	SOCKET connectedSocket= (SOCKET)stuff;
	int buflen= DEFAULT_BUFLEN;
	int iResult= 0;
	char* receiveBuffer= new char[DEFAULT_BUFLEN];
	do {
		for(int i=0; i<DEFAULT_BUFLEN; i++){
			receiveBuffer[i]= '\0';
		}
		iResult = recv(connectedSocket, receiveBuffer, buflen, 0);
    if (iResult > 0){
		//printf("Bytes received: %d\n", iResult);
		cout << "client received: " << (char*)receiveBuffer << '\n';
	}    
    else if (iResult == 0){
        printf("Connection closed?\n");
		bail=true;
	}
    else{
		printf("recv failed: %d\n", WSAGetLastError());
		bail=true;
	}
	} while (!bail);
	printf("CLIENT LOOP HAS ENDED");
}