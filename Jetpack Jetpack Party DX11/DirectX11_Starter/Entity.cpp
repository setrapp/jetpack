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
	totalMeshes = 0;
	transform = new Transform();
	XMStoreFloat4x4(&transform->trans, XMMatrixIdentity());
	XMStoreFloat4x4(&transform->scale, XMMatrixIdentity());
	XMStoreFloat4x4(&transform->rot, XMMatrixIdentity());
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
	Mesh* m = new Mesh(v, i, 6, 4);
	meshes.push_back(m);
	totalMeshes ++;
}

void Entity::AddTraingle(Vertex* v, UINT* i)
{
	Mesh* m = new Mesh(v, i, 3, 3);
	meshes.push_back(m);
	totalMeshes ++;
}

void Entity::Update(float dt)
{
	if(this->transform->changed)
	{
		this->transform->changed = false;
		XMStoreFloat4x4(&this->transform->worldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&transform->scale) * XMLoadFloat4x4(&transform->rot) * XMLoadFloat4x4(&transform->trans)));
	}
}

void Entity::Draw()
{
	if(material)
	{
		DXConnection::Instance()->deviceContext->PSSetShaderResources(0, 1, &material->resourceView);
		DXConnection::Instance()->deviceContext->PSSetSamplers(0, 1, &material->samplerState);
	}
	for(LONG i = 0; i < totalMeshes; i++)
	{		
		meshes[i]->Draw();
	}
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

std::vector<Mesh*> Entity::GetMeshes()
{
	return meshes;
}