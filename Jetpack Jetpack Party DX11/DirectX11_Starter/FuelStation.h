#pragma once
#include "Entity.h"

class FuelStation : public Entity
{
public:
	FuelStation(float refuelRate, int checkpointNum = -1);
	void Update(float dt);
	float GetRefuelRate();
	int GetCheckpointNum();
	bool IsCheckpoint();

public:
	bool spin;

protected:
	float refuelRate;
	int checkpointNum;
};