#include "NavMeshSegment.h"

NavMeshSegment::NavMeshSegment(int segmentNum = -1)
{
	this->segmentNum = segmentNum;
}

void FindConnections(vector<NavMeshSegment*>* possibleConnections)
{
	if (!possibleConnections)
	{
		return;
	}
	
	for (vector<NavMeshSegment*>::iterator it = possibleConnections->begin(); it < possibleConnections->end(); it++)
	{
		int connectedVertexCount = 0;
		int vertexCount = (*it)->GetVertexCount();
		for (int j = 0; j < vertexCount; j++)
		{
			if 
		}
	}
}

int NavMeshSegment::GetCheckpointNum()
{
	return segmentNum;
}

vector<NavMeshSegment*> const* NavMeshSegment::GetConnections()
{
	return &connections;
}