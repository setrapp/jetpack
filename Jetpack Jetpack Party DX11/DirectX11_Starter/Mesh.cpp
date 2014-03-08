#include "Mesh.h"
#include "Vertex.h"
#include <stdio.h>

Mesh::Mesh(Vertex* v, UINT* i, int noOfIndices, int noOfVertices, ID3D11Device* device)
{
	D3D11_BUFFER_DESC vbd;
    vbd.Usage					= D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth				= sizeof(Vertex) * noOfVertices; // Number of vertices
    vbd.BindFlags				= D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags			= 0;
    vbd.MiscFlags				= 0;
	vbd.StructureByteStride		= 0;

    D3D11_SUBRESOURCE_DATA initialVertexData;
    initialVertexData.pSysMem	= v;

    HR(device->CreateBuffer(
		&vbd,
		&initialVertexData,
		&vertexBuffer));

	// Create the index buffer
	D3D11_BUFFER_DESC ibd;
    ibd.Usage					= D3D11_USAGE_IMMUTABLE;
    ibd.ByteWidth				= sizeof(UINT) * noOfIndices; // Number of indices
    ibd.BindFlags				= D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags			= 0;
    ibd.MiscFlags				= 0;
	ibd.StructureByteStride		= 0;

    D3D11_SUBRESOURCE_DATA initialIndexData;
    initialIndexData.pSysMem	= i;

    HR(device->CreateBuffer(
		&ibd,
		&initialIndexData,
		&indexBuffer));
	totalIndices = noOfIndices;
}


Mesh::~Mesh(void)
{
	//ReleaseMacro(vertexBuffer);
	//ReleaseMacro(indexBuffer);
}

void Update(float dt);

void Mesh::Draw(ID3D11DeviceContext* deviceContext)
{	
	const UINT stride = sizeof(Vertex);
	UINT offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	deviceContext->DrawIndexed(
		this->totalIndices,	// The number of indices we're using in this draw
		0,
		0);
}