#include "ClientConnectionEntity.h"
queue<string> ClientConnectionEntity::networkMessages;
int ClientConnectionEntity::socketNum;
ClientConnectionEntity::ClientConnectionEntity(void)
{
	receivedChat = new std::queue<char*>();
	isConnected=false;
	networkMessages = queue<string>();
}

ClientConnectionEntity::~ClientConnectionEntity()
{
	// TODO: need to figure out how to deallocate memory	
}


void ClientConnectionEntity::sendMessage(string sentMessage){
	

	int iResult;
	// Send an initial buffer
	//iResult = send(ConnectSocket, sendbuf, (int) strlen(sendbuf), 0);
	sentMessage+="<EOF>";
	iResult = send(ConnectSocket, sentMessage.c_str(), 512, 0);
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
	if(!isConnected){
		isConnected=true;
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

		iResult = getaddrinfo(address.c_str(), DEFAULT_PORT, &hints, &result);
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

		freeaddrinfo(result);

		if (ConnectSocket == INVALID_SOCKET) {
			cout <<"Unable to connect to server!\n";
			WSACleanup();
		}	
		_beginthread(ClientConnectionEntity::listenForResponse,0,(void*)ConnectSocket);
	}


}


void ClientConnectionEntity::listenForResponse(void* stuff){
	bool bail= false;
	SOCKET connectedSocket= (SOCKET)stuff;
	int buflen= DEFAULT_BUFLEN;
	int iResult= 0;
	char* receiveBuffer= new char[DEFAULT_BUFLEN];
	string s= "SOCKET REQUEST<EOF>";
	send(connectedSocket, s.c_str(), 512, 0);
	int counter=0;
	do {
		for(int i=0; i<DEFAULT_BUFLEN; i++){
			receiveBuffer[i]= '\0';
		}
		iResult = recv(connectedSocket, receiveBuffer, buflen, 0);
		if (iResult > 0){

			string toParse(receiveBuffer);
			string curString;
			cout << "client received: " << (char*)receiveBuffer << '\n';
			if(toParse.find("ADDED. SOCKET NUMBER IS: ",0)!=-1){
				string socketAddedStr="ADDED. SOCKET NUMBER IS: ";
				toParse = toParse.substr(socketAddedStr.length());
				std::vector<std::string> stringParts;
				std::istringstream stringsplitter(toParse);
				while(std::getline(stringsplitter,curString, '\n')){
					stringParts.push_back(curString);
				}
				string socketString=(string)stringParts.at(0);
				ClientConnectionEntity::socketNum=atoi(socketString.c_str());
				string socketNumLengthEndLn = to_string(ClientConnectionEntity::socketNum);
				socketNumLengthEndLn +="\n";
				toParse = toParse.substr(socketNumLengthEndLn.length());


				string currentPlayersString= "CURRENT PLAYERS: ";
				toParse = toParse.substr(currentPlayersString.length());

				string toPush= "1";
				toPush+= "\n";
				ostringstream ss;
				ss<< toParse;
				toPush+= ss.str();
						
				networkMessages.push(toPush);
			}
			else if(toParse.find("SOCKET ADDED: ",0)!=-1){
				string socketAddedStr= "SOCKET ADDED: ";
				toParse = toParse.substr(socketAddedStr.length());
				int value = atoi(toParse.c_str());
				if(value != ClientConnectionEntity::socketNum){
					string toPush= "2";
					toPush+= "\n";
					ostringstream ss;
					ss<< value;
					toPush+= ss.str();
						
					networkMessages.push(toPush);
				}
			}
			else if(toParse.find("MOVEMENT UPDATE: ",0)!=-1){
				string movementUpdateStr= "MOVEMENT UPDATE: ";
				toParse = toParse.substr(movementUpdateStr.length());
				string toPush= "3";
				toPush+= "\n";
				toPush+= toParse;
				networkMessages.push(toPush);
			}
			
			counter++;
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