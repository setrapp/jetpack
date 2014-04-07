#include "GreaterMesh.h"

GMesh* GMesh::instance = NULL;
GMesh* GMesh::Instance()
{
	return instance;
}

GMesh::GMesh() 
{
	if (!instance)
	{
		instance = this;
		instance->totalIndices = 0;
		instance->totalVertices = 0;
	}
}

GMesh::~GMesh()
{
	if (instance == this)
	{
		instance = NULL;
	}
}


inline void GMesh::SaveMeshData(Vertex* v, UINT* i, int noOfIndices, int noOfVertices)
{
	instance->totalIndices += noOfIndices;
	instance->totalVertices += noOfVertices;

	for(int j = 0; j < noOfIndices; j++)
	{
		instance->indices.push_back(i[j]);
	}

	for(int j = 0; j < noOfVertices; j++)
	{
		instance->vertices.push_back(v[j]);
	}
}

void GMesh::FinalizeData()
{
	D3D11_BUFFER_DESC vbd;
    vbd.Usage					= D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth				= sizeof(Vertex) * instance->totalVertices; // Number of vertices
    vbd.BindFlags				= D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags			= 0;
    vbd.MiscFlags				= 0;
	vbd.StructureByteStride		= 0;

	size_t a = sizeof(Vertex);

    D3D11_SUBRESOURCE_DATA initialVertexData;
	Vertex* v = (Vertex*)calloc(instance->totalVertices, sizeof(Vertex));
	for(int j =0; j < instance->totalVertices; j++) 
	{
		v[j] = instance->vertices.at(j);
	}
    initialVertexData.pSysMem	= v;

    HR(DXConnection::Instance()->device->CreateBuffer(
		&vbd,
		&initialVertexData,
		&instance->vertexBuffer));

	// Create the index buffer
	D3D11_BUFFER_DESC ibd;
    ibd.Usage					= D3D11_USAGE_IMMUTABLE;
    ibd.ByteWidth				= sizeof(UINT) * instance->totalIndices; // Number of indices
    ibd.BindFlags				= D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags			= 0;
    ibd.MiscFlags				= 0;
	ibd.StructureByteStride		= 0;

    D3D11_SUBRESOURCE_DATA initialIndexData;
	UINT* i = (UINT*)calloc(instance->totalIndices, sizeof(UINT));
	for(int j =0; j < instance->totalIndices; j++) 
	{
		i[j] = instance->indices.at(j);
	}
    initialIndexData.pSysMem	= i;

    HR(DXConnection::Instance()->device->CreateBuffer(
		&ibd,
		&initialIndexData,
		&instance->indexBuffer));
}

void GMesh::BufferedDraw() {
	const UINT stride = sizeof(Vertex);
	UINT offset = 0;

	DXConnection::Instance()->deviceContext->IASetVertexBuffers(0, 1, &instance->vertexBuffer, &stride, &offset);
	DXConnection::Instance()->deviceContext->IASetIndexBuffer(instance->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	DXConnection::Instance()->deviceContext->DrawIndexed(
		instance->totalIndices,	// The number of indices we're using in this draw
		0,
		0);
}
