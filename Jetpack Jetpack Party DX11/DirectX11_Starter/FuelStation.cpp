#include "FuelStation.h"

FuelStation::FuelStation(float refuelRate, int checkpointNum) : Entity()
{
	this->refuelRate = refuelRate;
	this->checkpointNum = checkpointNum;
	spin = false;
}

void FuelStation::Update(float dt)
{
	Entity::Update(dt);

	if (spin)
	{
		transform.Rotate(XMFLOAT3(0, 1 * dt, 0));
	}
}

float FuelStation::GetRefuelRate()
{
	return refuelRate;
}

int FuelStation::GetCheckpointNum()
{
	return checkpointNum;
}

bool FuelStation::IsCheckpoint()
{
	return checkpointNum >= 0;
}