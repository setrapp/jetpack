#pragma once
#define WIN32_LEAN_AND_MEAN
#include "Entity.h"
#include "Camera.h"
#include "Player.h";
#include "ClientConnectionEntity.h"
#include "UDPClientConnectionEntity.h"
#include "MessageTypes.h"
#include <queue>
#include "Jetpack.h"
#include "ManeuverJetpack.h"
#include "PositionLerp.h"
#include "NetworkedPlayer.h"
class NetworkManager
{
public:
	NetworkManager(Player** p);
	~NetworkManager(void);
	vector<string>* breakIntoParts(string s);
	//ClientConnectionEntity* clientEntity;
	UDPClientConnectionEntity* clientEntity;
	std::map<int,NetworkedPlayer*> networkedEntities;
	std::map<int,XMFLOAT3> networkedEntityVelocities;
	std::map<int,PositionLerp> networkedEntityLerps;
	float networkSendTimer;
	Player* player;
	void AddNewUser(int playerIndex);
	float updateTimer;
	bool loggedIn;
	Jetpack* jetpack;
	void CheckInput(float dt);
	void Update(float dt);
	int playerSocketNumber;

	enum Thruster
	{
		BOTTOM_LEFT = 0,
		BOTTOM_RIGHT,
		BACK_LEFT,
		BACK_RIGHT,
		FRONT_LEFT,
		FRONT_RIGHT,
		SIDE_LEFT,
		SIDE_RIGHT
	};
};

