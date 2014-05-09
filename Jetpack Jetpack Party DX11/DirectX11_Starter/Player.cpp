#include "Player.h"
#include "AssetManager.h"

Player::Player()
{
	minPosture = 0.95f;
	respawnPosition = XMFLOAT3(0, 0, 0);
	respawnLocalRotation = XMFLOAT3(0, 0, 0);
	worldVelocity = XMFLOAT3(0, 0, 0);
	angularVelocity = XMFLOAT3(0, 0, 0);
	maxSpeed = 200;
	forwardAcceleration = 100.0f;
	backwardAcceleration = 100.0f;
	strafeAcceleration = 100.0f;
	gravityAcceleration = 600.0f;
	terminalVelocity = 500000;
	groundSpeedDampening = 0.1f;
	airSpeedDampening = 0.3f;
	grounded = true;
	jetpack = new ManeuverJetpack(this);
	clientEntity = new ClientConnectionEntity();
	clientEntity->connectClient("127.0.0.1");
	networkSendTimer=0.0f;
	loggedIn=false;
	controllable = true;

	// TEMP
	controllable = false;
	targetPosition = XMFLOAT3(100, 500, 3000);
}

Player::~Player()
{
	delete jetpack;
	delete clientEntity;
	delete networkedCube;
}

void Player::Update(float dt)
{
	//TODO account for networked players.
	jetpack->playerControllable = controllable;
	jetpack->playerAI = !controllable;

	XMFLOAT3 velocity = transform.InverseTransformDirection(worldVelocity);

	// Figure out the optimal acceleration to the target position.
	// TODO should only be done if AI controlled.
	XMVECTOR desiredDirection = XMVectorSubtract(XMLoadFloat3(&targetPosition), XMLoadFloat3(&transform.GetTranslation()));
	XMStoreFloat3(&jetpack->targetAcceleration, XMVectorSubtract(desiredDirection, XMLoadFloat3(&velocity)));

	// Check for user input.
	if (controllable)
	{
		CheckInput(dt);
	}
	// Update jetpack.
	jetpack->allowInputForces = !grounded; //TODO This might not be useful anymore
	jetpack->Update(dt, &velocity, &angularVelocity);

	// Clamp velocity withing max speed.
	transform.ClampVector(&velocity, (grounded ? maxSpeed : jetpack->maxSpeed), 0);

	// Update world velocity and apply world space accelerations.
	worldVelocity = transform.TransformDirection(velocity);

	// TODO gravity should be handled by rigid body.
	XMFLOAT3 position = transform.GetTranslation();
	if (position.y > 0)
	{
		grounded = false;
		if (worldVelocity.y > -terminalVelocity)
		{
			XMFLOAT3 gravity = XMFLOAT3(0, worldVelocity.y - (gravityAcceleration * dt), 0);
			transform.ClampVector(&gravity, terminalVelocity, 0);
			worldVelocity.y = gravity.y;
		}
	} 
	else if (worldVelocity.y <= 0 && (position.y < 0 || !grounded))
	{
		// When landing, either respawn or stand straight up, depending on standing direction.
		XMFLOAT3 upDot;
		XMStoreFloat3(&upDot, XMVector3Dot(XMLoadFloat3(&transform.GetUp()), XMLoadFloat3(&XMFLOAT3(0, 1, 0))));
		if (upDot.x < minPosture)
		{
			Respawn();
		}
		else
		{
			grounded = true;
			worldVelocity.y = 0;
			transform.SetLocalRotation(XMFLOAT3(0, transform.GetLocalEulerAngles().y, 0));
			angularVelocity = XMFLOAT3(0, 0, 0);
		}
	}
	velocity = transform.InverseTransformDirection(worldVelocity);

	// Apply world velocity.
	XMFLOAT3 dtVelocity;
	XMStoreFloat3(&dtVelocity, XMVectorScale(XMLoadFloat3(&velocity), dt));
	transform.Translate(dtVelocity);

	// Apply world angular velocity.
	XMFLOAT3 dtAngularVelocity;
	XMStoreFloat3(&dtAngularVelocity, XMVectorScale(XMLoadFloat3(&angularVelocity), dt));
	transform.Rotate(dtAngularVelocity);

	// Slow the character a bit so that it comes to a nice stop over time.
	if (grounded) {
		XMStoreFloat3(&velocity, XMVectorScale(XMLoadFloat3(&velocity), (1 - ((1 - groundSpeedDampening) * dt))));
		XMStoreFloat3(&angularVelocity, XMVectorScale(XMLoadFloat3(&angularVelocity), (1 - ((1 - groundSpeedDampening) * dt))));
	} else {
		XMStoreFloat3(&velocity, XMVectorScale(XMLoadFloat3(&velocity), (1 - ((1 - airSpeedDampening) * dt))));
		XMStoreFloat3(&angularVelocity, XMVectorScale(XMLoadFloat3(&angularVelocity), (1 - ((1 - airSpeedDampening) * dt))));
	}

	// Stop character if not moving much.
	XMFLOAT3 velMag;
	XMStoreFloat3(&velMag, XMVector3LengthSq(XMLoadFloat3(&velocity)));
	if (velMag.x > 0 && velMag.x < 0.001f)
	{
		velocity.x = velocity.y = velocity.z = 0;
	}
	// Stop character if not rotating much.
	XMFLOAT3 angVelMag;
	XMStoreFloat3(&angVelMag, XMVector3LengthSq(XMLoadFloat3(&angularVelocity)));
	if (angVelMag.x > 0 && angVelMag.x < 0.000000001f)
	{
		angularVelocity.x = angularVelocity.y = angularVelocity.z = 0;
	}

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

				networkedEntities[targetSocket]->transform.SetLocalRotation(newRotMatrix);
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
					networkedEntities[targetSocket]->transform.SetLocalRotation(newRotMatrix);



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
	if (grounded)
	{
		/*if(IPMan::GetIPMan()->GetKey(KeyType::FORWARD))
		{
			velocity.z += forwardAcceleration * dt;
			cubeInputReceived=true;
		}
		if(IPMan::GetIPMan()->GetKey(KeyType::BACKWARD))
		{
			velocity.z -= backwardAcceleration * dt;
			cubeInputReceived=true;
		}
		if(IPMan::GetIPMan()->GetKey(KeyType::LEFT))
		{
			velocity.x -= strafeAcceleration * dt;
			cubeInputReceived=true;
		}
		if(IPMan::GetIPMan()->GetKey(KeyType::RIGHT))
		{
			velocity.x += strafeAcceleration * dt;
			cubeInputReceived=true;
		}*/

		// TODO should use IPMan
		/*if (GetAsyncKeyState(VK_SPACE))
		{
			// Reset Rotation
			transform.SetLocalRotation(XMFLOAT3(0, 0, 0));
			angularVelocity = (XMFLOAT3(0, 0, 0));
			//grounded = false;
		}*/
	}


	if(GetAsyncKeyState('V'))
	{
		if(clientEntity->isConnected && !loggedIn){
			clientEntity->sendMessage(MessageTypes::Client::Login,"");
			loggedIn=true;
		}
	}

	if(clientEntity && clientEntity->isConnected && networkSendTimer<0.0f && cubeInputReceived && loggedIn){
		XMFLOAT3 curTransform= transform.GetTranslation();
		networkSendTimer=0.2f;
		clientEntity->sendMessage(MessageTypes::Client::MovementUpdate,getNetworkString());
	}
}

void Player::Respawn()
{
	transform.SetTranslation(respawnPosition);
	transform.SetLocalRotation(respawnLocalRotation);
	worldVelocity = XMFLOAT3(0, 0, 0);
	angularVelocity = XMFLOAT3(0, 0, 0);
	jetpack->Refuel(Jetpack::MAX_FUEL);
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