#include "Player.h"
#include "DemoGame.h"

Player::Player()
{
	velocity = XMFLOAT3(0, 0, 0);
	maxSpeed = 200;
	forwardAcceleration = 100.0f;
	backwardAcceleration = 100.0f;
	strafeAcceleration = 100.0f;
	gravityAcceleration = 100.0f;
	terminalVelocity = 300;
	groundSpeedDampening = 0.1f;
	airSpeedDampening = 0.7f;
	clientEntity = new ClientConnectionEntity();
	networkSendTimer=0.0f;
	grounded = true;
	jetpack = new Jetpack();
}

Player::~Player()
{
	delete jetpack;
	delete clientEntity;
	delete networkedCube;
}

void Player::Update(float dt)
{
	// Check for user input.
	CheckInput(dt);

	// Update jetpack.
	jetpack->allowInputForces = !grounded;
	jetpack->Update(dt, &velocity);

	// Clamp velocity withing max speed.
	transform.ClampVector(&velocity, (grounded ? maxSpeed : jetpack->maxSpeed), 0);

	// Apply world space accelerations.
	XMFLOAT3 worldVelocity = transform.TransformDirection(velocity);

	// TODO gravity should be handled by rigid body.
	//if (!jetpack->active)
	//{
		XMFLOAT3 position = transform.GetTranslation();
		if (position.y > 0)
		{
			if (worldVelocity.y > -terminalVelocity)
			{
				XMFLOAT3 gravity = XMFLOAT3(0, worldVelocity.y - (gravityAcceleration * dt), 0);
				transform.ClampVector(&gravity, terminalVelocity, 0);
				worldVelocity.y = gravity.y;
			}
		} 
		else if (!grounded && worldVelocity.y < 0)
		{
			grounded = true;
			transform.SetTranslation(XMFLOAT3(position.x, 0, position.z));
			worldVelocity.y = 0;
		}
	//}
	velocity = transform.InverseTransformDirection(worldVelocity);

	// Apply world velocity.
	XMFLOAT3 dtVelocity;
	XMStoreFloat3(&dtVelocity, XMVectorScale(XMLoadFloat3(&velocity), dt));
	transform.Translate(dtVelocity);

	// Slow the character a bit so that it comes to a nice stop over time.
	if (grounded) {
		XMStoreFloat3(&velocity, XMVectorScale(XMLoadFloat3(&velocity), (1 - ((1 - groundSpeedDampening) * dt))));
	} else {
		XMStoreFloat3(&velocity, XMVectorScale(XMLoadFloat3(&velocity), (1 - ((1 - airSpeedDampening) * dt))));
	}

	// Stop character if not moving much.
	XMFLOAT3 velMag;
	XMStoreFloat3(&velMag, XMVector3LengthSq(XMLoadFloat3(&velocity)));
	if (velMag.x > 0 && velMag.x < 0.001f)
	{
		velocity.x = velocity.y = velocity.z = 0;
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
		switch(toSwitch){
			//add all players if you are entering the games
		case 1:
			for(int i=1; i<(int)stringParts.size()-1; i+=2){
				int targetSocket= atoi(stringParts.at(i).c_str());

				string unparsedPosition= stringParts.at(i+1);

				cube = new Entity();
				cube->socketNumber= targetSocket;
				cube->AddModel(AssetManager::Instance()->GetModel("jetman"));
				cube->Finalize();
				cube->transform.Translate(XMFLOAT3(0, -4, 0));
				cube->transform.Rotate(XMFLOAT3(0, PI / 2, 0));
				networkedEntities[cube->socketNumber]=cube;
				AssetManager::Instance()->addedEntities.push(cube);

				std::vector<std::string> vectorParts;

				string vectorString;
				std::istringstream stringsplitter(unparsedPosition);
				while(std::getline(stringsplitter,vectorString, ',')){
					vectorParts.push_back(vectorString);
				}

				XMFLOAT3 newVector= XMFLOAT3(strtod(vectorParts.at(0).c_str(),0),strtod(vectorParts.at(1).c_str(),0),strtod(vectorParts.at(2).c_str(),0));
				XMFLOAT3 currentTransform=networkedEntities[targetSocket]->transform.GetLocalTranslation();
				//calculates how much to translate the body in question
				networkedEntities[targetSocket]->transform.Translate(XMFLOAT3(newVector.x-currentTransform.x,newVector.y-currentTransform.y,newVector.z-currentTransform.z));
			}


			break;
			//adding this current player
		case 2:
			cube = new Entity();
			cube->socketNumber= atoi(stringParts.at(1).c_str());
			cube->AddModel(AssetManager::Instance()->GetModel("jetman"));
			cube->Finalize();
			cube->transform.Translate(XMFLOAT3(0, -4, 0));
			cube->transform.Rotate(XMFLOAT3(0, PI / 2, 0));
			cube->transform.SetParent(&this->transform);
			networkedEntities[cube->socketNumber]=cube;
			AssetManager::Instance()->addedEntities.push(cube);
			break;

			//a list of sockets and the positions of the data is sent, the positions of all the associated entities is updated.
		case 3:
			for(int i=1; i<(int)stringParts.size()-1; i+=2){
				int targetSocket= atoi(stringParts.at(i).c_str());
				if(networkedEntities[targetSocket]!=NULL){
					string unparsedPosition= stringParts.at(i+1);

					std::vector<std::string> vectorParts;

					string vectorString;
					std::istringstream stringsplitter(unparsedPosition);
					while(std::getline(stringsplitter,vectorString, ',')){
						vectorParts.push_back(vectorString);
					}

					XMFLOAT3 newVector= XMFLOAT3(strtod(vectorParts.at(0).c_str(),0),strtod(vectorParts.at(1).c_str(),0),strtod(vectorParts.at(2).c_str(),0));
					XMFLOAT3 currentTransform=networkedEntities[targetSocket]->transform.GetLocalTranslation();

					//calculates how much to translate the body in question
					networkedEntities[targetSocket]->transform.Translate(XMFLOAT3(newVector.x-currentTransform.x,newVector.y-currentTransform.y,newVector.z-currentTransform.z));
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
		if(IPMan::GetIPMan()->GetKey(KeyType::FORWARD))
		{
			velocity.z += forwardAcceleration * dt;
		}
		if(IPMan::GetIPMan()->GetKey(KeyType::BACKWARD))
		{
			velocity.z -= backwardAcceleration * dt;
		}
		if(IPMan::GetIPMan()->GetKey(KeyType::LEFT))
		{
			velocity.x -= strafeAcceleration * dt;
		}
		if(IPMan::GetIPMan()->GetKey(KeyType::RIGHT))
		{
			velocity.x += strafeAcceleration * dt;
		}

		// TODO should use IPMan
		if (GetAsyncKeyState(VK_SPACE))
		{
			grounded = false;
		}
	}


	if(GetAsyncKeyState('V'))
	{
		if(!clientEntity->isConnected){
			Entity* cube = new Entity();
			cube->AddModel(AssetManager::Instance()->GetModel("jetman"));
			cube->Finalize();
			cube->transform.Translate(XMFLOAT3(0, -4, 0));
			cube->transform.Rotate(XMFLOAT3(0, PI / 2, 0));
			networkedCube= cube;
			AssetManager::Instance()->addedEntities.push(cube);

			clientEntity->connectClient("127.0.0.1");
		}
	}

	if(GetAsyncKeyState(VK_NUMPAD1)){
		if(clientEntity->isConnected){
			cubeInputReceived=true;
			networkedCube->transform.Translate(XMFLOAT3(dt, 0, 0));

		}

	}

	if(GetAsyncKeyState(VK_NUMPAD5)){
		if(clientEntity->isConnected){
			cubeInputReceived=true;
			networkedCube->transform.Translate(XMFLOAT3(0, 0, dt));
		}

	}

	if(GetAsyncKeyState(VK_NUMPAD2)){
		if(clientEntity->isConnected){
			cubeInputReceived=true;
			networkedCube->transform.Translate(XMFLOAT3(0, 0,-1* dt));
		}

	}

	if(GetAsyncKeyState(VK_NUMPAD3)){
		if(clientEntity->isConnected){
			cubeInputReceived=true;
			networkedCube->transform.Translate(XMFLOAT3(-1* dt, 0, 0));
		}
	}

	if(clientEntity && clientEntity->isConnected && networkSendTimer<0.0f && cubeInputReceived){
		XMFLOAT3 curTransform= networkedCube->transform.GetLocalTranslation();
		networkSendTimer=0.2f;
		std::ostringstream ss1;
		ss1 << curTransform.x;
		std::string s1(ss1.str());
		std::ostringstream ss2;
		ss2 << curTransform.y;
		std::string s2(ss2.str());
		std::ostringstream ss3;
		ss3 << curTransform.z;
		std::string s3(ss3.str());

		clientEntity->sendMessage(s1+","+ s2+","+ s3);

	}
}