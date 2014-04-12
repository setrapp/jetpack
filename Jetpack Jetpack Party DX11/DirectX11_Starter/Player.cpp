#include "Player.h"

Player::Player()
{
	velocity = XMFLOAT3(0, 0, 0);
	maxSpeed = 20;
	groundSpeedDampening = 0.95f;
	
}

Player::Player(DemoGame* v_gp)
{
	velocity = XMFLOAT3(0, 0, 0);
	maxSpeed = 20;
	groundSpeedDampening = 0.95f;
	game= v_gp;
	clientEntity = new ClientConnectionEntity();
	networkedEntities = vector<Entity*>();
	
}

void Player::Update(float dt)
{
	// Check for user input.
	CheckInput(dt);
	
	// Slow the character a bit so that it comes to a nice stop over time.
	XMStoreFloat3(&velocity, XMVectorScale(XMLoadFloat3(&velocity), groundSpeedDampening));
	XMFLOAT3 transformedVel;
	//XMVECTOR velVec;
	XMStoreFloat3(&transformedVel, XMVector3Transform(XMLoadFloat3(&XMFLOAT3(velocity.x * dt, velocity.y * dt, velocity.z * dt)), XMLoadFloat3x3(&transform->GetRotation())));
	transform->Translate(transformedVel);
	DirectX::XMFLOAT3 t =transform->GetLocalTranslation();

	while(!clientEntity->networkMessages.empty()){
		std::vector<std::string> stringParts;
		string readData= clientEntity->networkMessages.front();
		string curString;
		std::istringstream stringsplitter(readData);
		while(std::getline(stringsplitter,curString, '\n')){
			stringParts.push_back(curString);
		}
		int toSwitch= atoi(stringParts.at(0).c_str());
		switch(toSwitch){
		case 1:

			break;
			//a new player has been added
		case 2:
			AssetManager::Instance()->CreateAndStoreMesh("../Assets/cube.obj", "cube");
			Entity* cube = new Entity();
			cube->socketNumber= atoi(stringParts.at(1).c_str());
			cube->AddMesh(AssetManager::Instance()->GetMesh("cube"));
			cube->transform->Translate(XMFLOAT3(-5, 0, 0));
			cube->transform->SetParent(this->transform);
			networkedEntities[cube->socketNumber]=cube;
			game->addedEntities.push(cube);
			clientEntity->networkMessages.pop();
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
					XMFLOAT3 currentTransform=networkedEntities[targetSocket]->transform->GetLocalTranslation();

					//calculates how much to translate the body in question
					networkedEntities[targetSocket]->transform->Translate(XMFLOAT3(newVector.x-currentTransform.x,newVector.y-currentTransform.y,newVector.z-currentTransform.z));
				}

			}
		}
			
	}


	Entity::Update(dt);
}

void Player::CheckInput(float dt)
{
	if(GetAsyncKeyState(VK_UP))
	{
		velocity.z += 0.8f;
	}
	if(GetAsyncKeyState(VK_DOWN))
	{
		velocity.z -= 0.8f;
	}
	if(GetAsyncKeyState(VK_LEFT))
	{
		velocity.x -= 0.8f;
	}
	if(GetAsyncKeyState(VK_RIGHT))
	{
		velocity.x += 0.8f;
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

	if(GetAsyncKeyState('A'))
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

	if(GetAsyncKeyState('E'))
	{
		transform->Rotate(XMFLOAT3(0, 0, 1 * dt));
	}

	//spawns a block at a preset location
	if(GetAsyncKeyState('P')){
		
	}

	if(GetAsyncKeyState('N')){
	}
	if(GetAsyncKeyState('M')){
		if(!clientEntity->isConnected){
			AssetManager::Instance()->CreateAndStoreMesh("../Assets/cube.obj", "cube");
			Entity* cube = new Entity();
			cube->AddMesh(AssetManager::Instance()->GetMesh("cube"));
			cube->transform->Translate(XMFLOAT3(-5, 0, 0));
			cube->transform->SetParent(this->transform);
			networkedCube= cube;
			game->addedEntities.push(cube);

			clientEntity->connectClient("127.0.0.1");
		}
	}

	if(GetAsyncKeyState('H')){
		if(clientEntity->isConnected)
			networkedCube->transform->Translate(XMFLOAT3(dt, 0, 0));

	}
	if(GetAsyncKeyState('U')){
		if(clientEntity->isConnected)
			networkedCube->transform->Translate(XMFLOAT3(0, 0, dt));

	}
	if(GetAsyncKeyState('J')){
		if(clientEntity->isConnected)
			networkedCube->transform->Translate(XMFLOAT3(0, 0,-1* dt));

	}
	if(GetAsyncKeyState('K')){
		if(clientEntity->isConnected)
			networkedCube->transform->Translate(XMFLOAT3(-1* dt, 0, 0));
	}


	if(GetAsyncKeyState('X'))
	{
		transform->Scale(XMFLOAT3(1 + dt, 1 + dt, 1 + dt));
	}
	if(GetAsyncKeyState('Z'))
	{
		transform->Scale(XMFLOAT3(1 - dt, 1 - dt, 1 - dt));
	}
}