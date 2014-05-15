#pragma once
#include "Entity.h"

struct NavMeshConnection;

class NavMeshSegment : public Entity
{
public:
	NavMeshSegment(int segmentId);
	~NavMeshSegment();
	
	void FindConnections(vector<NavMeshSegment*>* possibleConnections);
	vector<NavMeshConnection*> const* GetConnections();

private:
	int segmentId;
	float connectionProximity;
	vector<NavMeshConnection*> connections;
};

struct NavMeshConnection
{
	NavMeshSegment* firstSegment;
	NavMeshSegment* secondSegment;
	vector<XMFLOAT3> connections;
};