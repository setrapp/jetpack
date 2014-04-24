#pragma once

#include "Vertex.h"
#include <DirectXMath.h>
#include <d3d11.h>
#include "Common.h"

class Mesh
{
public:
	Mesh(UINT* i);
	UINT* GetIndices();

private:	
	UINT indices[3];
};

struct Model 
{
	vector<Vertex> vertices;
	vector<Mesh*> meshes;
};                                                                                                                                                                                                                                                                                                      