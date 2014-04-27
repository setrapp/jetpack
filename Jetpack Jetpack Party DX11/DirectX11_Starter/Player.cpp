#include "Player.h"
#include "DemoGame.h"

Player::Player()
{
	velocity = XMFLOAT3(0, 0, 0);
	maxSpeed = 20;
	groundSpeedDampening = 0.95f;
	clientEntity = new ClientConnectionEntity();
	clientEntity->connectClient("138.91.117.6");
	networkSendTimer=0.0f;
	loggedIn=false;
	
}

Player::~Player()
{
	delete clientEntity;
	delete networkedCube;
}

void Player::Update(float dt)
{
	// Check for user input.
	CheckInput(dt);
	
	// Slow the character a bit so that it comes to a nice stop over time.
	XMStoreFloat3(&velocity, XMVectorScale(XMLoadFloat3(&velocity), groundSpeedDampening));
	transform.Translate(transform.InverseTransformDirection(XMFLOAT3(velocity.x * dt, velocity.y * dt, velocity.z * dt)));

	while(!clientEntity->networkMessages.empty()){

		
		string readData= clientEntity->networkMessages.front();
		string curString;
		Entity* cube;
		std::vector<std::string> stringParts;
		std::istringstream stringsplitter(readData);
		while(std::getline(stringsplitter,curString, '\n')){
			stringParts.push_back(curString);
		}
		int toSwitch= atoi(stringParts.at(0).c_str());
		MessageTypes::Server msgType= (MessageTypes::Server)toSwitch;

		switch(msgType){
			//add all players if you are entering the games
		case MessageTypes::Server::AddExistingUsers:

			this->socketNumber= atoi(stringParts.at(1).c_str());

			for(int i=2; i<(int)stringParts.size()-1; i+=2){
				int targetSocket= atoi(stringParts.at(i).c_str());

				string unparsedPosition= stringParts.at(i+1);
				cube = new Entity();
				cube->socketNumber= targetSocket;
				cube->AddModel(AssetManager::Instance()->GetModel("jetman"));
				cube->Finalize();
				cube->transform.Translate(XMFLOAT3(0, 0, 0));
				cube->transform.Rotate(XMFLOAT3(0, PI / 2, 0));
				networkedEntities[cube->socketNumber]=cube;
				AssetManager::Instance()->addedEntities.push(cube);

				std::vector<std::string>* vectorParts = breakIntoParts(unparsedPosition);

				XMFLOAT3 newVector= XMFLOAT3(strtod(vectorParts->at(0).c_str(),0),strtod(vectorParts->at(1).c_str(),0),strtod(vectorParts->at(2).c_str(),0));
				XMFLOAT3 currentTransform=networkedEntities[targetSocket]->transform.GetTranslation();
				//calculates how much to translate the body in question
				networkedEntities[targetSocket]->transform.Translate(XMFLOAT3(newVector.x-currentTransform.x,newVector.y-currentTransform.y,newVector.z-currentTransform.z));


				XMFLOAT3X3 newRotMatrix= XMFLOAT3X3(strtod(vectorParts->at(3).c_str(),0),strtod(vectorParts->at(4).c_str(),0),strtod(vectorParts->at(5).c_str(),0),
												  strtod(vectorParts->at(6).c_str(),0),strtod(vectorParts->at(7).c_str(),0),strtod(vectorParts->at(8).c_str(),0),
												  strtod(vectorParts->at(9).c_str(),0),strtod(vectorParts->at(10).c_str(),0),strtod(vectorParts->at(11).c_str(),0));

				networkedEntities[targetSocket]->transform.rotation=newRotMatrix;
			}


			break;
			//adding a player that recently joined
		case MessageTypes::Server::AddNewUser:
			loggedIn=true;
			cube = new Entity();
			cube->AddModel(AssetManager::Instance()->GetModel("jetman"));
			cube->Finalize();
			cube->transform.Translate(XMFLOAT3(0, 0, 0));
			cube->transform.Rotate(XMFLOAT3(0, PI / 2, 0));
			cube->socketNumber=atoi(stringParts.at(1).c_str());
			networkedEntities[cube->socketNumber]=cube;
			AssetManager::Instance()->addedEntities.push(cube);
			break;

			//a list of sockets and the positions of the data is sent, the positions of all the associated entities is updated.
		case MessageTypes::Server::MovementUpdate:
			for(int i=1; i<(int)stringParts.size()-1; i+=2){
				int targetSocket= atoi(stringParts.at(i).c_str());
				if((networkedEntities[targetSocket]!=NULL) && targetSocket!=this->socketNumber){
					string unparsedPosition= stringParts.at(i+1);

					std::vector<std::string>* vectorParts = breakIntoParts(unparsedPosition);

					XMFLOAT3 newVector= XMFLOAT3(strtod(vectorParts->at(0).c_str(),0),strtod(vectorParts->at(1).c_str(),0),strtod(vectorParts->at(2).c_str(),0));
					XMFLOAT3 currentTransform=networkedEntities[targetSocket]->transform.GetTranslation();

					//calculates how much to translate the body in question
					networkedEntities[targetSocket]->transform.Translate(XMFLOAT3(newVector.x-currentTransform.x,newVector.y-currentTransform.y,newVector.z-currentTransform.z));
					XMFLOAT3X3 newRotMatrix= XMFLOAT3X3(strtod(vectorParts->at(3).c_str(),0),strtod(vectorParts->at(4).c_str(),0),strtod(vectorParts->at(5).c_str(),0),
												  strtod(vectorParts->at(6).c_str(),0),strtod(vectorParts->at(7).c_str(),0),strtod(vectorParts->at(8).c_str(),0),
												  strtod(vectorParts->at(9).c_str(),0),strtod(vectorParts->at(10).c_str(),0),strtod(vectorParts->at(11).c_str(),0));
					networkedEntities[targetSocket]->transform.rotation=newRotMatrix;



				}

			}
		}
		clientEntity->networkMessages.pop();
			
	}

	Entity::Update(dt);
}

void Player::CheckInput(float dt)
{
	networkSendTimer-=dt;
	bool cubeInputReceived= false;
	if(IPMan::GetIPMan()->GetKey(KeyType::FORWARD))
	{
		cubeInputReceived=true;
		velocity.z += 0.8f;
	}
	if(IPMan::GetIPMan()->GetKey(KeyType::BACKWARD))
	{
		cubeInputReceived=true;
		velocity.z -= 0.8f;
	}
	if(IPMan::GetIPMan()->GetKey(KeyType::LEFT))
	{
		cubeInputReceived=true;
		velocity.x -= 0.8f;
	}
	if(IPMan::GetIPMan()->GetKey(KeyType::RIGHT))
	{
		cubeInputReceived=true;
		velocity.x += 0.8f;
	}


	if(GetAsyncKeyState('V'))
	{
		if(clientEntity->isConnected && !loggedIn){
			clientEntity->sendMessage(MessageTypes::Client::Login,"");
			loggedIn=true;
		}
	}

	// Clamp to max speed.
	XMVECTOR velocityVec = XMLoadFloat3(&velocity);
	XMFLOAT3 velocityMag3;
	XMStoreFloat3(&velocityMag3, XMVector3Length(velocityVec));
	float velocityMag = velocityMag3.x;
	if (velocityMag > maxSpeed) {
		velocity.x = velocity.x * (maxSpeed / velocityMag);
		velocity.y = velocity.y * (maxSpeed / velocityMag);
		velocity.z = velocity.z * (maxSpeed / velocityMag);
	}

	if(clientEntity && clientEntity->isConnected && networkSendTimer<0.0f && cubeInputReceived && loggedIn){
		XMFLOAT3 curTransform= transform.GetTranslation();
		networkSendTimer=0.2f;
		clientEntity->sendMessage(MessageTypes::Client::MovementUpdate,getNetworkString());

	}
}

vector<string>* Player::breakIntoParts(string s){
	vector<string>* vectorParts= new vector<string>();
	string vectorString;
	std::istringstream stringsplitter(s);
	while(std::getline(stringsplitter,vectorString, ',')){
		vectorParts->push_back(vectorString);
	}
	return vectorParts;
}

