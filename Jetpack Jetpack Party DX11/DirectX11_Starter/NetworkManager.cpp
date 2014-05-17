#include "NetworkManager.h"
#include "AssetManager.h"

NetworkManager::NetworkManager(Player** p)
{
	player=*p;
	updateTimer=0.1f;
	jetpack = new ManeuverJetpack(player);
	clientEntity = new UDPClientConnectionEntity();
	clientEntity->connectClient("127.0.0.1");
	networkSendTimer=0.0f;
	clientEntity->sendMessage(MessageTypes::Client::Login,"");
	loggedIn=true;
}


void NetworkManager::Update(float dt){
	CheckInput(dt);
	//Lagging LERP system update
	std::map<int, PositionLerp>::iterator iter;
	for (iter = networkedEntityLerps.begin(); iter != networkedEntityLerps.end(); iter++) {
		XMFLOAT3 newPos= networkedEntityLerps[iter->first].getNewPosition(dt,updateTimer);
		XMFLOAT3 currentPosition=networkedEntities[iter->first]->transform.GetLocalTranslation();
		networkedEntities[iter->first]->transform.Translate(XMFLOAT3(newPos.x-currentPosition.x,newPos.y-currentPosition.y,newPos.z-currentPosition.z));
    }

	while(!clientEntity->networkMessages.empty()){

		
		string readData= clientEntity->networkMessages.front();
		string curString;
		NetworkedPlayer* cube;
		std::vector<std::string> stringParts;
		std::istringstream stringsplitter(readData);
		while(std::getline(stringsplitter,curString, '\n')){
			stringParts.push_back(curString);
		}

		int toSwitch= atoi(stringParts.at(0).c_str());
		MessageTypes::Server msgType= (MessageTypes::Server)toSwitch;

		int messageCount= atoi(stringParts.at(1).c_str());
		if((messageCount>clientEntity->lastMessageCountReceived) || msgType!=MessageTypes::Server::MovementUpdate || messageCount==0){
		clientEntity->lastMessageCountReceived=messageCount;
		switch(msgType){
			//add all players if you are entering the games
		case MessageTypes::Server::AddExistingUsers:
			
			playerSocketNumber= atoi(stringParts.at(1).c_str());
			player->socketNumber=playerSocketNumber;
			for(int i=3; i<(int)stringParts.size()-1; i+=2){
				int targetSocket= atoi(stringParts.at(i).c_str());

				string unparsedPosition= stringParts.at(i+1);
				AddNewUser(targetSocket);


				std::vector<std::string>* vectorParts = breakIntoParts(unparsedPosition);

				XMFLOAT3 newVector= XMFLOAT3(strtod(vectorParts->at(0).c_str(),0),strtod(vectorParts->at(1).c_str(),0),strtod(vectorParts->at(2).c_str(),0));
				XMFLOAT3 currentTransform=networkedEntities[targetSocket]->transform.GetTranslation();
				//calculates how much to translate the body in question
				networkedEntities[targetSocket]->transform.Translate(XMFLOAT3(newVector.x-currentTransform.x,newVector.y-currentTransform.y,newVector.z-currentTransform.z));


				XMFLOAT3X3 newRotMatrix= XMFLOAT3X3(strtod(vectorParts->at(3).c_str(),0),strtod(vectorParts->at(4).c_str(),0),strtod(vectorParts->at(5).c_str(),0),
												  strtod(vectorParts->at(6).c_str(),0),strtod(vectorParts->at(7).c_str(),0),strtod(vectorParts->at(8).c_str(),0),
												  strtod(vectorParts->at(9).c_str(),0),strtod(vectorParts->at(10).c_str(),0),strtod(vectorParts->at(11).c_str(),0));

				networkedEntities[targetSocket]->transform.SetLocalRotation(newRotMatrix);
			}


			break;
			//adding a player that recently joined
		case MessageTypes::Server::AddNewUser:
			loggedIn=true;
			AddNewUser(atoi(stringParts.at(2).c_str()));
			break;

			//a list of sockets and the positions of the data is sent, the positions of all the associated entities is updated.
		case MessageTypes::Server::MovementUpdate:
			for(int i=2; i<(int)stringParts.size()-1; i+=2){
				int targetSocket= atoi(stringParts.at(i).c_str());
				if((networkedEntities[targetSocket]!=NULL) && targetSocket!=playerSocketNumber){
					string unparsedPosition= stringParts.at(i+1);

					std::vector<std::string>* vectorParts = breakIntoParts(unparsedPosition);

					//XMFLOAT3 velocityVector= XMFLOAT3(strtod(vectorParts->at(0).c_str(),0),strtod(vectorParts->at(1).c_str(),0),strtod(vectorParts->at(2).c_str(),0));
					//networkedEntityVelocities[targetSocket] = velocityVector;
					((ManeuverJetpack*)networkedEntities[targetSocket]->jetpack)->updateThrusters(atoi(vectorParts->at(0).c_str()));

					XMFLOAT3 receivedVector= XMFLOAT3(strtod(vectorParts->at(1).c_str(),0),strtod(vectorParts->at(2).c_str(),0),strtod(vectorParts->at(3).c_str(),0));
					XMFLOAT3 currentPosition=networkedEntities[targetSocket]->transform.GetLocalTranslation();
					networkedEntityLerps[targetSocket].setNewTarget(receivedVector, currentPosition);

					//reset the lerps
					XMFLOAT3 newPosition=networkedEntityLerps[targetSocket].getNewPosition(dt,updateTimer);

					XMFLOAT3X3 newRotMatrix= XMFLOAT3X3(strtod(vectorParts->at(4).c_str(),0),strtod(vectorParts->at(5).c_str(),0),strtod(vectorParts->at(6).c_str(),0),
												  strtod(vectorParts->at(7).c_str(),0),strtod(vectorParts->at(8).c_str(),0),strtod(vectorParts->at(9).c_str(),0),
												  strtod(vectorParts->at(10).c_str(),0),strtod(vectorParts->at(11).c_str(),0),strtod(vectorParts->at(12).c_str(),0));
					networkedEntities[targetSocket]->transform.SetLocalRotation(newRotMatrix);
				}
			}
		}
		clientEntity->networkMessages.pop();
			
		}
	}
}


void NetworkManager::CheckInput(float dt){
	networkSendTimer-=dt;

	if(clientEntity && clientEntity->isConnected && networkSendTimer<0.0f && loggedIn){
		XMFLOAT3 curTransform= player->transform.GetTranslation();
		networkSendTimer=updateTimer + networkSendTimer;
		string curNetworkString= player->getNetworkString();
		int thrusterData=0;
		ManeuverJetpack* mJetpack= (ManeuverJetpack*)(player->jetpack);
		if(mJetpack->thrusterActives[Thruster::BOTTOM_RIGHT])
			thrusterData++;
		if(mJetpack->thrusterActives[Thruster::BACK_RIGHT])
			thrusterData+=2;
		if(mJetpack->thrusterActives[Thruster::SIDE_RIGHT])
			thrusterData+=4;
		if(mJetpack->thrusterActives[Thruster::FRONT_RIGHT])
			thrusterData+=8;
		if(mJetpack->thrusterActives[Thruster::BOTTOM_LEFT])
			thrusterData+=16;
		if(mJetpack->thrusterActives[Thruster::BACK_LEFT])
			thrusterData+=32;
		if(mJetpack->thrusterActives[Thruster::SIDE_LEFT])
			thrusterData+=64;
		if(mJetpack->thrusterActives[Thruster::FRONT_LEFT])
			thrusterData+=128;

		string toAddString= to_string(thrusterData) + ",";
		curNetworkString = toAddString + curNetworkString;
		clientEntity->sendMessage(MessageTypes::Client::MovementUpdate,curNetworkString);
	}

}



vector<string>* NetworkManager::breakIntoParts(string s){
	vector<string>* vectorParts= new vector<string>();
	string vectorString;
	std::istringstream stringsplitter(s);
	while(std::getline(stringsplitter,vectorString, ',')){
		vectorParts->push_back(vectorString);
	}
	return vectorParts;
}

void NetworkManager::AddNewUser(int playerIndex){

	NetworkedPlayer* newPlayer = new NetworkedPlayer();
	newPlayer->AddModel(AssetManager::Instance()->GetModel());
	newPlayer->Finalize();
	newPlayer->socketNumber=playerIndex;
	networkedEntities[newPlayer->socketNumber]=newPlayer;
	networkedEntityVelocities[newPlayer->socketNumber]= XMFLOAT3(0, 0, 0);
	networkedEntityLerps[newPlayer->socketNumber]= PositionLerp();
	AssetManager::Instance()->addedEntities.push(newPlayer);
	
	Entity* jetman = new Entity();
	jetman->AddModel(AssetManager::Instance()->GetModel("jetman"));
	jetman->Finalize();
	jetman->transform.Rotate(XMFLOAT3(0, PI / 2, 0));
	jetman->transform.SetParent(&newPlayer->transform);
	jetman->transform.SetLocalTranslation(XMFLOAT3(0, 0, 0));
	jetman->transform.Translate(XMFLOAT3(0, -5, 0));
	AssetManager::Instance()->addedEntities.push(jetman);

	newPlayer->jetpack->isNetworked=true;
	
	if (newPlayer->jetpack->thrusterActives)
	{
		for (int i = 0; i < newPlayer->jetpack->thrusterCount; i++)
		{
			if (newPlayer->jetpack->thrusters[i])
			{
				AssetManager::Instance()->addedEntities.push(newPlayer->jetpack->thrusters[i]);
			}
		}
	}

}

NetworkManager::~NetworkManager(void)
{
}
