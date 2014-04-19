#pragma once 
#include "Mesh.h"
#include "Vertex.h"
#include <stdio.h>

Mesh::Mesh(UINT* i, short noOfIndices)
{
	// TODO does this line need to exist
	indices = (UINT*)calloc(noOfIndices, sizeof(UINT));
	indices = i;
	totalIndices = noOfIndices;
}


Mesh::~Mesh(void)
{
	delete indices;
}

UINT* Mesh::GetIndices(short* total)
{
	*total = totalIndices;
	return indices;
}