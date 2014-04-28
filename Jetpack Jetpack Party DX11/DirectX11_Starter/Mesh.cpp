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

UINT* Mesh::GetIndices()
{
	return indices;
}

Material* Mesh::GetMaterial()
{
	return material;
}

void Mesh::SetMaterial(Material* newMaterial)
{
	material = newMaterial;
}