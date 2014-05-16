#include "NavMeshSegment.h"

NavMeshSegment::NavMeshSegment(int segmentId) : Entity()
{
	this->segmentId = segmentId;
	connectionProximity = 0.000001f;
}

NavMeshSegment::~NavMeshSegment()
{
	// To properly delete connections, first set one segement to NULL, then delete on the second.
	for (int i = 0; i < connections.size(); i++)
	{
		if (connections[i]->firstSegment)
		{
			if (!connections[i]->secondSegment)
			{
				delete connections[i];
			}
			else
			{
				connections[i]->firstSegment = NULL;
			}
		}
		else
		{
			if (!connections[i]->firstSegment)
			{
				delete connections[i];
			}
			else
			{
				connections[i]->secondSegment = NULL;
			}
		}
	}
}

#include "Debug.h"
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
								Debug::Log("" + this->segmentId + ' ' + (*it)->segmentId + '\n');
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

bool NavMeshSegment::EntityInside(Entity* entity)
{
	XMFLOAT3 posInSegment = transform.InverseTransformPoint(entity->transform.GetTranslation());
	/*bool outsideVertex = false;
	for (int i = 0; i < vertices.size() && !outsideVertex; i++)
	{
		XMFLOAT3 vertToPos;
		XMStoreFloat3(&vertToPos, XMVector3Normalize(XMVectorSubtract(XMLoadFloat3(&posInSegment), XMLoadFloat3(&vertices[i].Position))));
		XMFLOAT3 vertNorm = vertices[i].Normal;
		if (vertToPos.x > vertToPos.y && vertToPos.x > vertToPos.z)
		{
			vertToPos.x = vertNorm.x = 0;
		}
		else if (vertToPos.y > vertToPos.z)
		{
			vertToPos.y = vertNorm.y = 0;
		}
		else
		{
			vertToPos.z = vertNorm.z = 0;
		}
		float toDotNorm;
		XMStoreFloat(&toDotNorm, XMVector3Dot(XMLoadFloat3(&vertToPos), XMLoadFloat3(&vertNorm)));
		if (toDotNorm < 0)
		{
			outsideVertex = true;
		}
	}
	return !outsideVertex;*/
	bool outsideFace = false;
	for (int i = 0; i < meshes.size() && !outsideFace; i++)
	{
		XMFLOAT3 facePos, faceNormal;
		UINT* indices = meshes[i]->GetIndices();
		XMStoreFloat3(&facePos, XMVectorScale(XMVectorAdd(XMVectorAdd(XMLoadFloat3(&vertices[indices[0]].Position), XMLoadFloat3(&vertices[indices[1]].Position)), XMLoadFloat3(&vertices[indices[2]].Position)), 0.3333f));
		XMStoreFloat3(&faceNormal, XMVectorScale(XMVectorAdd(XMVectorAdd(XMLoadFloat3(&vertices[indices[0]].Normal), XMLoadFloat3(&vertices[indices[1]].Normal)), XMLoadFloat3(&vertices[indices[2]].Normal)), 0.3333f));
		XMFLOAT3 faceToPos;
		XMStoreFloat3(&faceToPos, XMVector3Normalize(XMVectorSubtract(XMLoadFloat3(&posInSegment), XMLoadFloat3(&facePos))));
		/*if (vertToPos.x > vertToPos.y && vertToPos.x > vertToPos.z)
		{
			vertToPos.x = vertNorm.x = 0;
		}
		else if (vertToPos.y > vertToPos.z)
		{
			vertToPos.y = vertNorm.y = 0;
		}
		else
		{
			vertToPos.z = vertNorm.z = 0;
		}*/
		float toDotNorm;
		XMStoreFloat(&toDotNorm, XMVector3Dot(XMLoadFloat3(&faceToPos), XMLoadFloat3(&faceNormal)));
		if (toDotNorm < 0)
		{
			outsideFace= true;
		}
	}
	return !outsideFace;
}

vector<NavMeshConnection*> const* NavMeshSegment::GetConnections()
{
	return &connections;
}