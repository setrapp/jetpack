#include "NavMeshSegment.h"

inline float EstimateCost(NavMeshSegment* start, NavMeshSegment* destintation);
inline float EstimateCost(XMFLOAT3 start, XMFLOAT3 destintation);
inline void CostPriorityInsert(vector<NavMeshSegment*>* target, map<NavMeshSegment*, pair<XMFLOAT3, float>>* existingCosts, NavMeshSegment* insertee, float cost);
inline bool ReconstructPath(map<NavMeshSegment*, NavMeshSegment*>* pathBack, NavMeshSegment* destination, vector<NavMeshSegment*>* pathOut);
inline bool FindMeshesFromVertices(map<Vertex*, vector<Mesh*>*>* vertexMeshes, vector<Vertex*>* verticesIn, vector<Mesh*>* meshesOut);

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

	for (int i = 0; i < connectionDistances.size(); i++)
	{
		delete connectionDistances[i];
	}

	for (int i = 0; i < vertices.size(); i++)

	{
		delete vertexMeshes.at(&vertices[i]);
	}
}

void NavMeshSegment::Finalize()
{
	Entity::Finalize();

	// Give vertices links to meshes that use them.
	vertexMeshes.clear();
	for (int i = 0; i < vertices.size(); i++)
	{
		vertexMeshes.insert(pair<Vertex*, vector<Mesh*>*>(&vertices[i], new vector<Mesh*>()));
	}
	for (int i = 0; i < meshes.size(); i++)
	{
		UINT* meshIndices = meshes[i]->GetIndices();
		vertexMeshes[&vertices[meshIndices[0]]]->push_back(meshes[i]);
		vertexMeshes[&vertices[meshIndices[1]]]->push_back(meshes[i]);
		vertexMeshes[&vertices[meshIndices[2]]]->push_back(meshes[i]);
	}
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

		/*TODO find meshes with vertices*/
		if (!alreadyConnected)
		{
			NavMeshConnection* newConnection = NULL;
			vector<XMFLOAT3> connectedPositions;
			vector<Vertex*> firstConnectedVertices;
			vector<Vertex*> secondConnectedVertices;
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
						firstConnectedVertices.push_back(&vertices[j]);
						secondConnectedVertices.push_back((*it)->GetVertex(k));
						if (connectedPositions.size() >= 3)
						{
							if (!newConnection)
							{
								newConnection = new NavMeshConnection();
								newConnection->firstSegment = this;
								newConnection->secondSegment = *it;
								this->connections.push_back(newConnection);
								(*it)->connections.push_back(newConnection);
								newConnection->connectionVertices.push_back(connectedPositions[0]);
								newConnection->connectionVertices.push_back(connectedPositions[1]);
								newConnection->connectionVertices.push_back(connectedPositions[2]);
								XMStoreFloat3(&newConnection->centroid, XMVectorScale(XMVectorAdd(XMVectorAdd(XMLoadFloat3(&connectedPositions[0]), XMLoadFloat3(&connectedPositions[1])), XMLoadFloat3(&connectedPositions[2])), 1.0f / 3));
							}
							else
							{
								newConnection->connectionVertices.push_back(connectedPos);
								XMStoreFloat3(&newConnection->centroid, XMVectorScale(XMVectorAdd(XMVectorScale(XMLoadFloat3(&newConnection->centroid), (newConnection->connectionVertices.size() - 1)), XMLoadFloat3(&connectedPos)), 1.0f / newConnection->connectionVertices.size()));
							}
						}
					}
				}
			}

			// Find connected meshes for the connected vertices.
			if (newConnection)
			{
				FindMeshesFromVertices(&this->vertexMeshes, &firstConnectedVertices, &newConnection->firstMeshes);
				FindMeshesFromVertices(&(*it)->vertexMeshes, &secondConnectedVertices, &newConnection->secondMeshes);
			}
		}
	}
}

void NavMeshSegment::ComputeConnectionDistances()
{
	for (int i = 0; i < connections.size(); i++)
	{
		for (int j = i + 1; j < connections.size(); j++)
		{
			NavMeshConnectionDistance* newDistance = new NavMeshConnectionDistance();
			newDistance->firstConnection = connections[i];
			newDistance->secondConnection = connections[j];
			XMStoreFloat(&newDistance->averageDistance, XMVector3Length(XMVectorSubtract(XMLoadFloat3(&connections[j]->centroid), XMLoadFloat3(&connections[i]->centroid))));
			connectionDistances.push_back(newDistance);
		}
	}
}

bool NavMeshSegment::FindPathTo(NavMeshSegment* destintation, XMFLOAT3 startPosition, XMFLOAT3 destinationPosition, vector<NavMeshSegment*>* pathOut)
{
	// TODO Check if start position is within starting segement and destination is within destination segement (or just pass in destination position and find destination node

	if (!pathOut)
	{
		return false;
	}

	// A* time!!!
	vector<NavMeshSegment*> open;
	vector<NavMeshSegment*> closed;
	map<NavMeshSegment*, NavMeshSegment*> pathBack;
	map<NavMeshSegment*, pair<XMFLOAT3, float>> gScores;
	map<NavMeshSegment*, pair<XMFLOAT3, float>> fScores;

	gScores.insert(pair<NavMeshSegment*, pair<XMFLOAT3, float>>(this, pair<XMFLOAT3, float>(startPosition, 0)));
	fScores.insert(pair<NavMeshSegment*, pair<XMFLOAT3, float>>(this, pair<XMFLOAT3, float>(startPosition, gScores[this].second + EstimateCost(gScores[this].first, destinationPosition))));
	pathBack.insert(pair<NavMeshSegment*, NavMeshSegment*>(this, NULL));

	bool pathFound = false;
	open.push_back(this);
	while(!pathFound && open.size() > 0)
	{
		// If the destination has been found, reconstruct the path.
		NavMeshSegment* current = open[0];
		if (current == destintation)
		{
			ReconstructPath(&pathBack, current, pathOut);
		}

		// Move to current to closed list.
		open.erase(open.begin());
		closed.push_back(current);

		// Add neighbors to open list.
		XMFLOAT3 segmentEnterPos = gScores[current].first;
		for (int i = 0; i < current->connections.size(); i++)
		{
			// Pick the Neighbor
			NavMeshSegment* neighbor = current->connections[i]->firstSegment;
			if (neighbor == current)
			{
				neighbor = current->connections[i]->secondSegment;
			}

			// Ignore segments that have already been closed.
			if (find(closed.begin(), closed.end(), neighbor) == closed.end())
			{
				// Compute distance to the neighbor.
				float costToConnection;
				XMStoreFloat(&costToConnection, XMVector3Length(XMVectorSubtract(XMLoadFloat3(&current->connections[i]->centroid), XMLoadFloat3(&segmentEnterPos))));
				costToConnection += gScores[current].second;

				// If the neighbor has not been opened yet, open it and prioritize it base on cost to reach.
				if (find(open.begin(), open.end(), neighbor) == open.end())
				{
					gScores.insert(pair<NavMeshSegment*, pair<XMFLOAT3, float>>(neighbor, pair<XMFLOAT3, float>(current->connections[i]->centroid, costToConnection)));
					fScores.insert(pair<NavMeshSegment*, pair<XMFLOAT3, float>>(neighbor, pair<XMFLOAT3, float>(current->connections[i]->centroid, gScores[neighbor].second + EstimateCost(gScores[neighbor].first, destinationPosition))));
					pathBack.insert(pair<NavMeshSegment*, NavMeshSegment*>(neighbor, current));
					CostPriorityInsert(&open, &gScores, neighbor, gScores[neighbor].second);
				}
				else
				{
					// If the neighbor has already been opened and the current cost to reach it
					// is less the previous best cost, update the opening.
					if (costToConnection < gScores[neighbor].second)
					{
						gScores[neighbor] = pair<XMFLOAT3, float>(current->connections[i]->centroid, costToConnection);
						fScores[neighbor] = pair<XMFLOAT3, float>(current->connections[i]->centroid, gScores[neighbor].second + EstimateCost(gScores[neighbor].first, destinationPosition));
						pathBack[neighbor]= current;
						CostPriorityInsert(&open, &gScores, neighbor, gScores[neighbor].second);
					}
				}
			}	
		}
	}

	return pathFound;
}

bool NavMeshSegment::FindPathPositions(vector<NavMeshSegment*>* path, XMFLOAT3 startPosition, XMFLOAT3 destinationPosition, vector<XMFLOAT3>* positionsOut)
{
	if (!path || !positionsOut)
	{
		return false;
	}

	bool brokenPath = false;
	XMFLOAT3 prevPos = startPosition;
	for (int i = 0; i + 1 < path->size() && !brokenPath; i++)
	{
		NavMeshConnection* connection = NULL;
		NavMeshSegment* segment = path->at(i);
		NavMeshSegment* nextSegment = path->at(i + 1);
		for(int j = 0; j < segment->connections.size() && !connection; j++)
		{
			if ((segment->connections[j]->firstSegment == segment && segment->connections[j]->secondSegment == nextSegment) ||
				(segment->connections[j]->secondSegment == segment && segment->connections[j]->firstSegment == nextSegment))
			{
				connection = segment->connections[j];
			}
		}
		// TODO Find nearest point on each connecting face for both start and destination,
		// and weighted average each pair based on distance from either end.
		if (connection)
		{
			positionsOut->push_back(segment->transform.TransformPoint(connection->centroid));
			/*Mesh* connectionMesh = connection->firstMeshes[0];
			if (segment = connection->secondSegment)
			{
				connectionMesh = connection->secondMeshes[0];
			}
			positionsOut->push_back(segment->ProjectPointOnMesh(connectionMesh, prevPos, false));
			prevPos = positionsOut->at(positionsOut->size() - 1);
			//temp
			prevPos = segment->ProjectPointOnMesh(connectionMesh, prevPos, false);

			//temp
			XMFLOAT3 a = segment->transform.TransformPoint(connection->centroid);
			int b = 0;*/
		}
		else
		{
			brokenPath = true;
		}
	}
	if (!brokenPath)
	{
		positionsOut->push_back(destinationPosition);
	}

	return brokenPath;
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

		XMFLOAT3 faceNormal2;
		XMStoreFloat3(&faceNormal2, XMVector3Normalize(XMVector3Cross(
			XMVectorSubtract(XMLoadFloat3(&vertices[indices[1]].Position), XMLoadFloat3(&vertices[indices[0]].Position)), 
			XMVectorSubtract(XMLoadFloat3(&vertices[indices[2]].Position), XMLoadFloat3(&vertices[indices[0]].Position)))));

		XMFLOAT3 faceToPos;
		XMStoreFloat3(&faceToPos, XMVector3Normalize(XMVectorSubtract(XMLoadFloat3(&posInSegment), XMLoadFloat3(&facePos))));
		float toDotNorm;
		XMStoreFloat(&toDotNorm, XMVector3Dot(XMLoadFloat3(&faceToPos), XMLoadFloat3(&faceNormal2)));
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

vector<NavMeshConnectionDistance*> const* NavMeshSegment::GetConnectionDistances()
{
	return &connectionDistances;
}

// Non-Class Functions
float EstimateCost(NavMeshSegment* start, NavMeshSegment* destintation)
{
	return (EstimateCost(start->transform.GetTranslation(), destintation->transform.GetTranslation()));
}

float EstimateCost(XMFLOAT3 start, XMFLOAT3 destintation)
{
	float cost;
	XMStoreFloat(&cost, XMVector3Length(XMVectorSubtract(XMLoadFloat3(&start), XMLoadFloat3(&destintation))));
	return cost;
}

void CostPriorityInsert(vector<NavMeshSegment*>* target, map<NavMeshSegment*, pair<XMFLOAT3, float>>* existingCosts, NavMeshSegment* insertee, float cost)
{
	if (!target || !existingCosts)
	{
		return;
	}

	vector<NavMeshSegment*>::iterator inserteeIt = find(target->begin(), target->end(), insertee);
	if (inserteeIt != target->end())
	{
		target->erase(inserteeIt);
	}
	
	
	bool inserted = false;
	for (int i = 0; i < target->size(); i++)
	{
		if (cost < existingCosts->at(target->at(0)).second)
		{
			target->insert(target->begin() + i, insertee);
			inserted = true;
		}
	}
	if (!inserted)
	{
		inserted = true;
		target->push_back(insertee);
	}
}

bool ReconstructPath(map<NavMeshSegment*, NavMeshSegment*>* pathBack, NavMeshSegment* destination, vector<NavMeshSegment*>* pathOut)
{
	if (!pathBack || !pathOut)
	{
		return false;
	}

	NavMeshSegment* current = destination;
	while (pathBack->at(current))
	{
		pathOut->insert(pathOut->begin(), current);
		current = pathBack->at(current);
	}

	return true;
}

bool FindMeshesFromVertices(map<Vertex*, vector<Mesh*>*>* vertexMeshes, vector<Vertex*>* verticesIn, vector<Mesh*>* meshesOut)
{
	if (!vertexMeshes || !verticesIn || !meshesOut)
	{
		return false;
	}
	int vertCount = verticesIn->size();
	for (int i = 0; i < vertCount; i++)
	{
		vector<Mesh*>* meshesOnVert1 = vertexMeshes->at(verticesIn->at(i));
		int meshCount = meshesOnVert1->size();
		for (int m = 0; m < meshCount; m++)
		{
			for (int j = i + 1; j < vertCount; j++)
			{
				vector<Mesh*>* meshesOnVert2 = vertexMeshes->at(verticesIn->at(j));
				if (find(meshesOnVert2->begin(), meshesOnVert2->end(), meshesOnVert1->at(m)) != meshesOnVert2->end())
				{
					for (int k = j + 1; k < vertCount; k++)
					{
						vector<Mesh*>* meshesOnVert3 = vertexMeshes->at(verticesIn->at(k));
						if (find(meshesOnVert3->begin(), meshesOnVert3->end(), meshesOnVert1->at(m)) != meshesOnVert3->end())
						{
							meshesOut->push_back(meshesOnVert1->at(m));
						}
					}
				}
			}
		}
	}
}