#pragma once
#include "Common.h"
class PositionLerp
{
public:
	int playerIndex;
	float currentLerpPosition;
	
	XMFLOAT3 getNewPosition(float update, float updateRate);
	XMFLOAT3 setNewTarget(XMFLOAT3 newTarget, XMFLOAT3 currentPosition);

	PositionLerp(void);
	~PositionLerp(void);

private:
	XMFLOAT3 prevPosition;
	XMFLOAT3 currentPosition;
	XMFLOAT3 seekPosition;
};
