#pragma once

#include "Vertex.h"
#include <DirectXMath.h>
#include <d3d11.h>
#include "Common.h"
#include "Material.h"

class Mesh
{
public:
	Mesh(UINT* i);
	UINT* GetIndices();
	Material* GetMaterial();
	void SetMaterial(Material* newMaterial);

private:	
	UINT indices[3];
	Material* material;
};

struct Model 
{
	vector<Vertex> vertices;
	vector<Mesh> meshes;
};                                                                                                                                                                                                                                                                                                      