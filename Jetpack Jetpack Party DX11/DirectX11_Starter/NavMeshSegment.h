#pragma once
#include "Entity.h"

class NavMeshSegment : public Entity
{
public:
	NavMeshSegment(int segmentNum = -1);
	
	void FindConnections(vector<NavMeshSegment*>* possibleConnections);
	int GetCheckpointNum();
	vector<NavMeshSegment*> const* GetConnections();

private:
	float connectionProximity;
	int segmentNum;
	vector<NavMeshSegment*> connections;
};