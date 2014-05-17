#pragma once
#include "Entity.h"

struct NavMeshConnection;
struct NavMeshConnectionDistance;

class NavMeshSegment : public Entity
{
public:
	NavMeshSegment(int segmentId);
	~NavMeshSegment();
	
	void Finalize();
	void FindConnections(vector<NavMeshSegment*>* possibleConnections);
	void ComputeConnectionDistances();
	bool FindPathTo(NavMeshSegment* destintation, XMFLOAT3 startPosition, XMFLOAT3 destinationPosition, vector<NavMeshSegment*>* pathOut);
	bool FindPathPositions(vector<NavMeshSegment*>* path, XMFLOAT3 startPosition, XMFLOAT3 destinationPosition, vector<XMFLOAT3>* positionsOut);
	bool EntityInside(Entity* entity);

	vector<NavMeshConnection*> const* GetConnections();
	vector<NavMeshConnectionDistance*> const* GetConnectionDistances();

public:
	map<Vertex*, vector<Mesh*>*> vertexMeshes;

private:
	int segmentId;
	float connectionProximity;
	vector<NavMeshConnection*> connections;
	vector<NavMeshConnectionDistance*> connectionDistances;
};

struct NavMeshConnection
{
	NavMeshSegment* firstSegment;
	NavMeshSegment* secondSegment;
	vector<Mesh*> firstMeshes;
	vector<Mesh*> secondMeshes;
	vector<XMFLOAT3> connectionVertices;
	XMFLOAT3 centroid;
};

struct NavMeshConnectionDistance
{
	NavMeshConnection* firstConnection;
	NavMeshConnection* secondConnection;
	float averageDistance;
};