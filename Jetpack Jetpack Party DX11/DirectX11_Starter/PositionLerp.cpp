#include "PositionLerp.h"


PositionLerp::PositionLerp(void)
{
	playerIndex=-1;
	currentLerpPosition=0.0f;
	prevPosition= XMFLOAT3(0.0f,0.0f,0.0f);
	currentPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);
	seekPosition= XMFLOAT3(0.0f,0.0f,0.0f);
}
	XMFLOAT3 PositionLerp::getNewPosition(float update, float updateRate){
		float amountToUpdate= update/updateRate;
		currentLerpPosition += amountToUpdate;
		if(currentLerpPosition> 1.0f){
			currentPosition=seekPosition;
			return seekPosition;
		}
		else{
			//lerps to find the current position
			currentPosition= XMFLOAT3((prevPosition.x * (1.0f-currentLerpPosition)) + (seekPosition.x * currentLerpPosition),
									  (prevPosition.y * (1.0f-currentLerpPosition)) + (seekPosition.y * currentLerpPosition),
									  (prevPosition.z * (1.0f-currentLerpPosition)) + (seekPosition.z * currentLerpPosition));
			return currentPosition;
		}

		return XMFLOAT3(0,0,0);
	}
	XMFLOAT3 PositionLerp::setNewTarget(XMFLOAT3 newTarget, XMFLOAT3 curPosition){
		currentLerpPosition=0.0f;
		seekPosition= newTarget;
		prevPosition= curPosition;
		currentPosition= prevPosition;
		return currentPosition;
	}

	 
PositionLerp::~PositionLerp(void)
{
}
