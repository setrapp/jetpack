#include "NavMeshSegment.h"

NavMeshSegment::NavMeshSegment(int segmentId) : Entity()
{
	this->segmentId = segmentId;
	connectionProximity = 0.01f;
}

NavMeshSegment::~NavMeshSegment()
{
	// TODO Release connections from the segement know as the firstSegment.
}


void NavMeshSegment::FindConnections(vector<NavMeshSegment*>* possibleConnections)
{
	if (!possibleConnections)
	{
		return;
	}
	
	int vertexCount = GetVertexCount();
	float vertexDistanceSqr = 0;
	float connectionProximitySqr = connectionProximity * connectionProximity;
	for (vector<NavMeshSegment*>::iterator it = possibleConnections->begin(); it < possibleConnections->end(); it++)
	{
		bool alreadyConnected = (this == *it);
		if (!alreadyConnected)
		{
			for (vector<NavMeshConnection*>::iterator ct = connections.begin(); ct < connections.end() && !alreadyConnected; ct++)
			{
				if ((*ct)->firstSegment == *it || (*ct)->secondSegment == *it)
				{
					alreadyConnected = true;
				}
			}
		}

		if (!alreadyConnected)
		{
			NavMeshConnection* newConnection = NULL;
			vector<XMFLOAT3> connectedPositions;
			int otherVertexCount = (*it)->GetVertexCount();
			for (int j = 0; j < vertexCount; j++)
			{
				for (int k = 0; k < otherVertexCount; k++)
				{
					XMStoreFloat(&vertexDistanceSqr, XMVector3LengthSq(XMVectorSubtract(XMLoadFloat3(&vertices[j].Position), XMLoadFloat3(&(*it)->GetVertex(k)->Position))));
					if (vertexDistanceSqr <= connectionProximitySqr)
					{
						XMFLOAT3 connectedPos = vertices[j].Position;
						connectedPositions.push_back(connectedPos);
						if (connectedPositions.size() >= 3)
						{
							if (!newConnection)
							{
								newConnection = new NavMeshConnection();
								newConnection->firstSegment = this;
								newConnection->secondSegment = *it;
								this->connections.push_back(newConnection);
								(*it)->connections.push_back(newConnection);
								newConnection->connections.push_back(connectedPositions[0]);
								newConnection->connections.push_back(connectedPositions[1]);
								newConnection->connections.push_back(connectedPositions[2]);
							}
							else
							{
								newConnection->connections.push_back(connectedPos);
							}
						}
					}
				}
			}
		}
	}
}

vector<NavMeshConnection*> const* NavMeshSegment::GetConnections()
{
	return &connections;
}