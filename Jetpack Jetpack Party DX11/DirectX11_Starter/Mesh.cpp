#pragma once 
#include "Mesh.h"
#include "Vertex.h"
#include <stdio.h>

Mesh::Mesh(UINT* i)
{
	indices[0] = i[0];
	indices[1] = i[1];
	indices[2] = i[2];
}


Mesh::~Mesh(void)
{
	delete indices;
}

UINT* Mesh::GetIndices()
{
	return indices;
}