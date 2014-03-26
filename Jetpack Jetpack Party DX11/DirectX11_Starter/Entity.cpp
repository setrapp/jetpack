#include "Entity.h"
#include "Toolkit\Inc\WICTextureLoader.h"
#include <Windows.h>
#include <d3dcompiler.h>
#include "DemoGame.h"
#include "Common.h"
#include <stdio.h>
#include <stdlib.h>
using namespace DirectX;

Entity::Entity(ID3D11Device* device)
{
	totalMeshes = 0;
	transform = new Transform();
	XMStoreFloat4x4(&transform->trans, XMMatrixIdentity());
	XMStoreFloat4x4(&transform->scale, XMMatrixIdentity());
	XMStoreFloat4x4(&transform->rot, XMMatrixIdentity());
	this->device = device;
}


Entity::~Entity(void)
{
}

XMFLOAT4X4 Entity::GetWorldMatrix()
{
	return this->transform->worldMatrix;
}

void Entity::AddQuad(Vertex* v, UINT* i)
{
	Mesh* m = new Mesh(v, i, 6, 4, this->device);
	meshes.push_back(m);
	totalMeshes ++;
}

void Entity::AddTraingle(Vertex* v, UINT* i)
{
	Mesh* m = new Mesh(v, i, 3, 3, this->device);
	meshes.push_back(m);
	totalMeshes ++;
}

void Entity::Update(float dt, VertexShaderConstantBuffer* vsConstantBuffer)
{
	vsConstantBuffer->world = this->transform->worldMatrix;
	if(this->transform->changed)
	{
		this->transform->changed = false;
		XMStoreFloat4x4(&this->transform->worldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&transform->scale) * XMLoadFloat4x4(&transform->rot) * XMLoadFloat4x4(&transform->trans)));
	}
}

void Entity::Draw(ID3D11DeviceContext* deviceContext)
{
	if(material)
	{
		deviceContext->PSSetShaderResources(0, 1, &material->resourceView);
		deviceContext->PSSetSamplers(0, 1, &material->samplerState);
	}
	for(LONG i = 0; i < totalMeshes; i++)
	{		
		meshes[i]->Draw(deviceContext);
	}
}


void Entity::LoadTexture(wchar_t* path, ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{	
	/*Material* w = new Material(device, deviceContext, path);
	Material m(device, deviceContext, path);
	this->material = (Material*)malloc(sizeof(Material*));
	this->material->resourceView = m.resourceView;
	this->material->samplerState = m.samplerState;
	this->material->texture = m.texture;*/
	this->material->ApplyTexture(device, deviceContext, path);
}


Material* Entity::GetMaterial()
{
	return this->material;
}


Material Entity::GetMaterialSafe()
{
	return *this->material;
}

std::vector<Mesh*> Entity::GetMeshes()
{
	return meshes;
}