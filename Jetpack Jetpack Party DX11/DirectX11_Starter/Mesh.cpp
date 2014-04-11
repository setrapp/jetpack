#pragma once 
#include "Mesh.h"
#include "Vertex.h"
#include <stdio.h>
#include "../DirectX11_Starter/GreaterMesh.h"

Mesh::Mesh(UINT* i, short noOfIndices)
{
	indices = (UINT*)calloc(noOfIndices, sizeof(UINT));
	indices = i;
	totalIndices = noOfIndices;
}


Mesh::~Mesh(void)
{
	delete indices;
}

UINT* Mesh::GetIndices(short** total)
{
	*total = &totalIndices;
	return indices;
}
		

void Update(float dt);

void Mesh::Draw()
{	
	
}