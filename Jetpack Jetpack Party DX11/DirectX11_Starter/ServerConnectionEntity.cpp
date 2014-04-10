
#include "ServerConnectionEntity.h"

std::map <SOCKET, string >ServerConnectionEntity::testMap;
ServerConnectionEntity::ServerConnectionEntity(void)
{
	output = new queue<string>();
	iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed: %d\n", iResult);
	}
}

void ServerConnectionEntity::initializeServer(){
	struct addrinfo *result = NULL, *ptr = NULL, hints;

	ZeroMemory(&hints, sizeof (hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the local address and port to be used by the server
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed: %d\n", iResult);
		WSACleanup();
   
	}

	ListenSocket = INVALID_SOCKET;
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		printf("Error at socket(): %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
	}

	// Setup the TCP listening socket
    iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
    }

	freeaddrinfo(result);

	if ( listen( ListenSocket, SOMAXCONN ) == SOCKET_ERROR ) {
		printf( "Listen failed with error: %ld\n", WSAGetLastError() );
		closesocket(ListenSocket);
		WSACleanup();
	}

	SOCKET ClientSocket = INVALID_SOCKET;

	// Accept a client socket
	

	_beginthread(listenForConnections,0,(void*)ListenSocket);

	
}
void ServerConnectionEntity::listenForConnections(void* stuff){

SOCKET ListenSocket = (SOCKET)stuff;
	bool bail=false;
	while(!bail){
		SOCKET curSocket = accept(ListenSocket, NULL, NULL);
		if (curSocket == INVALID_SOCKET) {
			cout<< "accept failed: "<<WSAGetLastError() << '\n';
			closesocket(curSocket);
			WSACleanup();
			bail=true;
		}

		_beginthread(manageConnection,0,(void*)curSocket);
	}
}

void ServerConnectionEntity::manageConnection(void* stuff){
	int iResult=0, iSendResult;
	int recvbuflen = DEFAULT_BUFLEN;
	bool bail= false;
	SOCKET curSocket= (SOCKET)stuff;

	testMap[(int)curSocket] = "0";
	string toReturn= "your socket number is: ";
	toReturn.append(to_string((int)curSocket));
	send(curSocket,toReturn.c_str() , iResult, 0);
	// Receive until the peer shuts down the connection
	do {
		char* recvbuf = new char[DEFAULT_BUFLEN];
		for(int i=0; i<DEFAULT_BUFLEN; i++){
				recvbuf[i]= '\0';
			}
		iResult = recv(curSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0) {


			//printf("Bytes received: %d\n", iResult);
			char* received = (char*)recvbuf;
			cout << "server received: " << received << '\n';
			string str(received);
			if(str.compare("SOCKET REQUEST")== 0){
				const char* toSend;
				int test= (int)curSocket;
				string s_test= to_string(test);
				toSend=s_test.c_str();
				iSendResult = send(curSocket,toSend , iResult, 0);
			}
			else{

				testMap[curSocket] = received;
				string s= printPortData();

				// Echo the buffer back to the sender
				const char* toSend;
				toSend=s.c_str();
				std::map<SOCKET, string>::iterator iter;
				for (iter = testMap.begin(); iter != testMap.end(); ++iter) {
					iSendResult = send(iter->first,toSend , iResult, 0);
				}
			}

			if (iSendResult == SOCKET_ERROR) {
				printf("send failed: %d\n", WSAGetLastError());
				closesocket(curSocket);
				WSACleanup();
			}
			//printf("Bytes sent: %d\n", iSendResult);
		} else if (iResult == 0){
			printf("Connection closing?\n");
			bail=true;
		}
        
		else {
			printf("recv failed: %d\n", WSAGetLastError());
			closesocket(curSocket);
			WSACleanup();
			bail=true;
		}

	} while (!bail);

	printf("SERVER LOOP HAS ENDED");


}


void ServerConnectionEntity::chatThread(void* stuff){

}


string ServerConnectionEntity::printPortData(){
	std::map<SOCKET, string>::iterator iter;
	
	string toReturn= "";
	for (iter = testMap.begin(); iter != testMap.end(); ++iter) {
		std::cout << (int)iter->first << " => " << iter->second << '\n';
		string str;
		toReturn.append(to_string((int)iter->first));
		toReturn.append(" => ");
		str= iter->second;
		toReturn.append(str);
		toReturn.append("\n");
	}
	return toReturn;

}
