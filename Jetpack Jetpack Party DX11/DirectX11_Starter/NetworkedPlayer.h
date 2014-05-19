#pragma once
#define WIN32_LEAN_AND_MEAN
#include "Entity.h"
#include "Camera.h"
#include "ClientConnectionEntity.h"
#include "UDPClientConnectionEntity.h"
#include "MessageTypes.h"
#include <queue>
#include "Jetpack.h"
#include "ManeuverJetpack.h"
#include "PositionLerp.h"
class NetworkedPlayer : public Entity
{
public:
	NetworkedPlayer(void);
	~NetworkedPlayer(void);
	Jetpack* jetpack;
	string playerName;
	void Update(float dt);
};

