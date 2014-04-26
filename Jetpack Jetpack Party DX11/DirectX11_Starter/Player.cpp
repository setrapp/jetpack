#include "Player.h"
#include "DemoGame.h"

Player::Player()
{
	velocity = XMFLOAT3(0, 0, 0);
	maxSpeed = 20;
	terminalVelocity = 100;
	groundSpeedDampening = 0.95f;
	airSpeedDampening = 0.999f;
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
	jetpack->Update(&velocity);

	// Clamp velocity withing max speed.
	transform.ClampVector(&velocity, (grounded ? maxSpeed : jetpack->maxSpeed), 0);

	// TODO gravity should be handled by rigid body.
	XMFLOAT3 position = transform.GetTranslation();
	if (position.y > 0)
	{
		XMFLOAT3 gravity = transform.TransformDirection(XMFLOAT3(0, -0.15, 0));
		XMFLOAT3 fallingVelocity = velocity;
		fallingVelocity.x += gravity.x;
		fallingVelocity.y += gravity.y;
		fallingVelocity.z += gravity.z;
		transform.ClampVector(&fallingVelocity, terminalVelocity, 0);
		XMFLOAT3 fallingMag, velMag;
		XMStoreFloat3(&fallingMag, XMVector3LengthSq(XMLoadFloat3(&fallingVelocity)));
		XMStoreFloat3(&velMag, XMVector3LengthSq(XMLoadFloat3(&velocity)));
		if (fallingMag.x > velMag.x)
		{
			velocity.x = fallingVelocity.x;
			velocity.y = fallingVelocity.y;
			velocity.z = fallingVelocity.z;
		}
	} 
	else if (!grounded)
	{
		grounded = true;
		transform.SetTranslation(XMFLOAT3(position.x, 0, position.z));
	}

	// Slow the character a bit so that it comes to a nice stop over time.
	if (grounded) {
		XMStoreFloat3(&velocity, XMVectorScale(XMLoadFloat3(&velocity), groundSpeedDampening));
	} else {
		XMStoreFloat3(&velocity, XMVectorScale(XMLoadFloat3(&velocity), airSpeedDampening));
	}

	// Apply velocity.
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
			velocity.z += 0.8f;
		}
		if(IPMan::GetIPMan()->GetKey(KeyType::BACKWARD))
		{
			velocity.z -= 0.8f;
		}
		if(IPMan::GetIPMan()->GetKey(KeyType::LEFT))
		{
			velocity.x -= 0.8f;
		}
		if(IPMan::GetIPMan()->GetKey(KeyType::RIGHT))
		{
			velocity.x += 0.8f;
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

	/*if(GetAsyncKeyState('A'))
	{
		transform->Rotate(XMFLOAT3(0, -1 * dt, 0));
	}
	if(GetAsyncKeyState('D'))
	{
		transform->Rotate(XMFLOAT3(0, 1 * dt, 0));
	}
	if(GetAsyncKeyState('W'))
	{
		transform->Rotate(XMFLOAT3(-1 * dt, 0 , 0));
	}
	if(GetAsyncKeyState('S'))
	{
		transform->Rotate(XMFLOAT3(1 * dt, 0, 0));
	}
	if(GetAsyncKeyState('Q'))
	{
		transform->Rotate(XMFLOAT3(0, 0, -1 * dt));
	}
	if(GetAsyncKeyState('E'))
	{
		transform->Rotate(XMFLOAT3(0, 0, 1 * dt));
	}

	if(GetAsyncKeyState('X'))
	{
		transform->Scale(XMFLOAT3(1 + dt, 1 + dt, 1 + dt));
	}
	if(GetAsyncKeyState('Z'))
	{
		transform->Scale(XMFLOAT3(1 - dt, 1 - dt, 1 - dt));
	}*/


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