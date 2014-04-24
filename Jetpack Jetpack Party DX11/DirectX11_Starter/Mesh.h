#pragma once

#include "Vertex.h"
#include <DirectXMath.h>
#include <d3d11.h>
#include "Common.h"

class Mesh
{
public:
	Mesh(UINT* i);
	~Mesh(void);
	UINT* GetIndices();

private:	
	UINT* indices;
};

struct Model 
{
	vector<Vertex> vertices;
	vector<Mesh*> meshes;
};                                                                                                                                                                                                                                                                                                      