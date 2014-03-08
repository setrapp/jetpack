#pragma once

#include "Vertex.h"
#include <DirectXMath.h>
#include <d3d11.h>
#include "Common.h"

using namespace DirectX;

class Mesh
{
public:
	Mesh(Vertex* v, UINT* i, int noOfIndices, int noOfVertices, ID3D11Device* device);
	~Mesh(void);
	void Initialize(Vertex* v, UINT* i, int noOfIndices, int noOfVertices, ID3D11Device* device);
	void Update(float dt);
	void Draw(ID3D11DeviceContext* deviceContext);

private:
	ID3D11Buffer* indexBuffer;
	ID3D11Buffer* vertexBuffer;
	short totalIndices;
};

