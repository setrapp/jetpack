#ifndef MESH_H_
#define MESH_H_
#pragma once

#include "Vertex.h"
#include <DirectXMath.h>
#include <d3d11.h>
#include "Common.h"

using namespace DirectX;

class Mesh
{
public:
	Mesh(Vertex* v, UINT* i, int noOfIndices, int noOfVertices);
	~Mesh(void);
	void Initialize(Vertex* v, UINT* i, int noOfIndices, int noOfVertices);
	void Update(float dt);
	void Draw();

private:
#ifndef BUFFERED_STUFF
	ID3D11Buffer* indexBuffer;
	ID3D11Buffer* vertexBuffer;
	short totalIndices;
#endif
};

#endif