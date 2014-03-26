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
	ID3D11Buffer* indexBuffer;
	ID3D11Buffer* vertexBuffer;
	short totalIndices;
};

