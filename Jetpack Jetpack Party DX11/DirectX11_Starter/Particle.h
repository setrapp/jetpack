#pragma once
#include <d3d11.h>
#include "AssetManager.h"
#include "Common.h"
#include <DirectXMath.h>
#include "Entity.h"
#include <vector>
#include "Mesh.h"
#include "Material.h"

using namespace DirectX;
using namespace std;
typedef struct Particle {
	Mesh* mesh;
	Transform* transform;
	Material* material;
	float life;
};

class ParticleSystyem {
public : 
	long maxparticles;
	vector<Particle*> particles;
	XMFLOAT3 source;
	XMFLOAT3 speed;
	XMFLOAT3 size;
	vector<Vertex> allVertices;
	float maxLife;

	ParticleSystyem(long maxparticles, XMFLOAT3 source, XMFLOAT3 speed, XMFLOAT3 size, float life)
	{			
	this->maxparticles = maxparticles;
	this->size = size;
	this->source = source;
	this->speed = speed;
	this->maxLife = life;
	for(int i = 0; i < maxparticles; i++)
		{		
			Particle* particle = new Particle();
			particle->life = 0;
			Vertex vertices[] = 
			{
				{ XMFLOAT3(+1.0f, +1.0f, +0.0f), XMFLOAT3(0, 0, -1), XMFLOAT2(0, 0) },
				{ XMFLOAT3(-1.0f, -1.0f, +0.0f), XMFLOAT3(0, 0, -1), XMFLOAT2(1, 1) },
				{ XMFLOAT3(+1.0f, -1.0f, +0.0f), XMFLOAT3(0, 0, -1), XMFLOAT2(0, 1) },		
				{ XMFLOAT3(-1.0f, +1.0f, +0.0f), XMFLOAT3(0, 0, -1), XMFLOAT2(1, 0) },
			};

			for(int i = 0; i < 4; i++)
			{
				vertices[i].Position.x += rand() % 5 / 10.0f;
				vertices[i].Position.y += rand() % 5 / 10.0f;
				vertices[i].Position.z += rand() % 5 / 10.0f;
				/*vertices[i].Position.x /= 2;
				vertices[i].Position.y /= 2;
				vertices[i].Position.z /= 2;*/
				allVertices.push_back(vertices[i]);
			}

			UINT indices[] = { 0, 2, 1, 3, 0, 1 };
			particle->mesh = new Mesh(indices, 6);
			particle->transform = new Transform();
			particle->material = new Material(L"../Assets/dust.png");
			particles.push_back(particle);
		}			

		Finalize();
	}

	void ParticleSystyem::Update(float dt)
	{
		for(int i = 0 ; i < particles.size(); i ++)
		{
			XMFLOAT3 pos = XMFLOAT3(rand() * 0.01f * dt, rand() * 0.01f* dt, rand() * 0.01f* dt);		
		//	particles.at(i)->transform->Scale(XMFLOAT3(rand() % (int)size.x, rand() % (int)size.y, rand() % (int)size.z));
			particles.at(i)->transform->Translate(pos);
			particles.at(i)->life += dt;
			auto trans = particles.at(i)->transform->GetTranslation();
			if(particles.at(i)->life > maxLife)
			{
				Reset(particles.at(i));
			}
			particles.at(i)->transform->UpdateLocalAndWorld();
			//
		} 
	}

	void ParticleSystyem::Draw()
	{
		for(int i = 0 ; i < particles.size(); i ++)
		{
		if(particles.at(i)->material)
			{
				ID3D11DeviceContext* deviceContext = DXConnection::Instance()->deviceContext;
				deviceContext->VSSetShader(particles.at(i)->material->vertexShader, NULL, 0);
				deviceContext->PSSetShader(particles.at(i)->material->pixelShader, NULL, 0);
				deviceContext->PSSetShaderResources(0, 1, &particles.at(i)->material->resourceView);
				deviceContext->PSSetSamplers(0, 1, &particles.at(i)->material->samplerState);
			}	

			const UINT stride = sizeof(Vertex);
			UINT offset = 0;

			DXConnection::Instance()->deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
			DXConnection::Instance()->deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

			DXConnection::Instance()->deviceContext->DrawIndexed(
				6,	// The number of indices we're using in this draw
				0,
				0);
		}
	}


private: ID3D11Buffer* indexBuffer;
		 ID3D11Buffer* vertexBuffer;

		 void ParticleSystyem::Finalize()
		 {
			vector<UINT> indicesAll;
			for(int i = 0; i < particles.size(); i++)
			{
				short temp;
				UINT* indices = particles.at(i)->mesh->GetIndices(&temp);
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
			vbd.ByteWidth				= sizeof(Vertex) * allVertices.size(); // Number of vertices
			vbd.BindFlags				= D3D11_BIND_VERTEX_BUFFER;
			vbd.CPUAccessFlags			= 0;
			vbd.MiscFlags				= 0;
			vbd.StructureByteStride		= 0;

			D3D11_SUBRESOURCE_DATA initialVertexData;
			initialVertexData.pSysMem	= &allVertices.at(0);

			HR(DXConnection::Instance()->device->CreateBuffer(
				&vbd,
				&initialVertexData,
				&vertexBuffer));
		 }

		 void Reset(Particle* par)
		 {
			 XMFLOAT3 dest;
			 XMStoreFloat3(&dest, XMVectorNegate(XMLoadFloat3(&source)));
			 par->transform->Translate(dest);
		 }
};
