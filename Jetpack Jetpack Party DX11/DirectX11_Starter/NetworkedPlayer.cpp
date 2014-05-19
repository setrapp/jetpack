#include "NetworkedPlayer.h"


NetworkedPlayer::NetworkedPlayer(void)
{
	jetpack = new ManeuverJetpack(this);
	playerName="";
}


NetworkedPlayer::~NetworkedPlayer(void)
{
}

void NetworkedPlayer::Update(float dt){

}
