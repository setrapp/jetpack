#pragma once 
#include "Entity.h"
#include "Toolkit\Inc\WICTextureLoader.h"
#include <Windows.h>
#include <d3dcompiler.h>
#include "DemoGame.h"
#include "Common.h"
#include <stdio.h>
#include <stdlib.h>

using namespace DirectX;

Entity::Entity()
{
	totalIndices = 0;
	totalMeshes = 0;
	transform = new Transform();
	material = AssetManager::Instance()->GetMaterial();
}


Entity::~Entity(void)
{
	delete transform;
	ReleaseMacro(vertexBuffer);
	ReleaseMacro(indexBuffer);
}


XMFLOAT4X4 Entity::GetWorldMatrix()
{
	return this->transform->GetWorldMatrix();
}

void Entity::AddQuad(Vertex* v, UINT* i)
{
	for (int j = 0; j < 6; j++)
	{
		i[j] += vertices.size();
	}

	Mesh* m = new Mesh(i, 6);
	for(int i = 0; i < 4; i++)
		vertices.push_back(v[i]);
	meshes.push_back(m);
	totalMeshes ++;
}

void Entity::AddTriangle(Vertex* v, UINT* i)
{
	for (int j = 0; j < 3; j++)
	{
		i[j] += vertices.size();
	}

	Mesh* m = new Mesh(i, 3);
	for(int i = 0; i < 4; i++)
		vertices.push_back(v[i]);
	meshes.push_back(m);
	totalMeshes ++;
}

void Entity::AddModel(Model* model) {
	int oldVertexCount = vertices.size();
	for (int i = 0; i < model->vertices.size(); i++)
	{
		vertices.push_back(model->vertices[i]);
	}
	for (int i = 0; i < model->meshes.size(); i++)
	{
		short totalIndices;
		UINT* modelIndices = model->meshes[i]->GetIndices(&totalIndices);
		UINT* indices = new UINT[totalIndices];
		for (int j = 0; j < totalIndices; j++)
		{
			indices[j] = modelIndices[j] + oldVertexCount;
		}
		Mesh* mesh = new Mesh(indices, totalIndices);
		meshes.push_back(mesh);
		totalMeshes++;
	}
}

void Entity::Update(float dt)
{
	
}

void Entity::Draw()
{
	if(material)
	{
		ID3D11DeviceContext* deviceContext = DXConnection::Instance()->deviceContext;
		deviceContext->VSSetShader(material->vertexShader, NULL, 0);
		deviceContext->PSSetShader(material->pixelShader, NULL, 0);
		deviceContext->PSSetShaderResources(0, 1, &material->resourceView);
		deviceContext->PSSetSamplers(0, 1, &material->samplerState);
	}	

	const UINT stride = sizeof(Vertex);
	UINT offset = 0;

	DXConnection::Instance()->deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	DXConnection::Instance()->deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	DXConnection::Instance()->deviceContext->DrawIndexed(
		this->totalIndices,	// The number of indices we're using in this draw
		0,
		0);
}


void Entity::LoadTexture(wchar_t* path)
{	
	this->material->ApplyTexture(path);
}

Material* Entity::GetMaterial()
{
	return this->material;
}


Material Entity::GetMaterialSafe()
{
	return *this->material;
}

void Entity::SetMaterial(string name)
{
	material = AssetManager::Instance()->GetMaterial(name);
}

vector<Mesh*> Entity::GetMeshes()
{
	return meshes;
}

void Entity::Finalize()
{
	vector<UINT> indicesAll;
	for(int i = 0; i < totalMeshes; i++)
	{
		short temp;
		UINT* indices = meshes.at(i)->GetIndices(&temp);
		for(short j= 0; j < temp; j++)
		{
			indicesAll.push_back(indices[j]);
		}
	}

	long size = indicesAll.size();
	UINT* indexPtr = &indicesAll.at(0);

	//// Create the index buffer
	D3D11_BUFFER_DESC ibd;
    ibd.Usage					= D3D11_USAGE_IMMUTABLE;
    ibd.ByteWidth				= sizeof(UINT) * size; // Number of indices
    ibd.BindFlags				= D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags			= 0;
    ibd.MiscFlags				= 0;
	ibd.StructureByteStride		= 0;

    D3D11_SUBRESOURCE_DATA initialIndexData;
    initialIndexData.pSysMem	= indexPtr;

    HR(DXConnection::Instance()->device->CreateBuffer(
		&ibd,
		&initialIndexData,
		&indexBuffer));

	D3D11_BUFFER_DESC vbd;
    vbd.Usage					= D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth				= sizeof(Vertex) * vertices.size(); // Number of vertices
    vbd.BindFlags				= D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags			= 0;
    vbd.MiscFlags				= 0;
	vbd.StructureByteStride		= 0;

    D3D11_SUBRESOURCE_DATA initialVertexData;
    initialVertexData.pSysMem	= &vertices.at(0);

    HR(DXConnection::Instance()->device->CreateBuffer(
		&vbd,
		&initialVertexData,
		&vertexBuffer));

	this->totalIndices = size;
}