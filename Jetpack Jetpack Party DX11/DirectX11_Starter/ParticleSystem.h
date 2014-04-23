#pragma once 
#include <d3d11.h>
#include "Common.h"
#include "DXConnection.h"
#include <DirectXMath.h>
#include "Material.h"
#include "AssetManager.h"
#include "Transform.h"
#include <vector>
#include "Particle.h"
#include "Menu.h"
#include <utility>

using namespace DirectX;

class ParticleSystem
{
	public:
		vector<Particle*> particles;
		ParticleSystem(const string materialName, long maxParticles, XMFLOAT3 velocity, XMFLOAT3 basePosition, XMFLOAT2 size, FXMVECTOR color = Colors::White, float age = 1)
		   {
			   XMFLOAT4 c ;
			   XMStoreFloat4(&c, color);
			   long totali = 0;
			   
			   Vertex vert[] = 
			   {
				   	{ XMFLOAT3(+1.0f, +1.0f, +0.0f), XMFLOAT3(0, 0, -1), XMFLOAT2(0, 0) },
					{ XMFLOAT3(-1.0f, -1.0f, +0.0f), XMFLOAT3(0, 0, -1), XMFLOAT2(1, 1) },
					{ XMFLOAT3(+1.0f, -1.0f, +0.0f), XMFLOAT3(0, 0, -1), XMFLOAT2(0, 1) },		
					{ XMFLOAT3(-1.0f, +1.0f, +0.0f), XMFLOAT3(0, 0, -1), XMFLOAT2(1, 0) },
			   };
			   UINT index[] =  { 0, 2, 1, 3, 0, 1 };
				
			   for(int i = 0; i < maxParticles; i++)
			   {
					Particle* p = new Particle(materialName, basePosition, size, c, age, velocity);
					p->transform->Translate(basePosition);
					particles.push_back(p);

					for(UINT i : index)
					{
						indices.push_back(i + totali);
					}
					//Keep track of indices over 6. 
					totali+=6;
					for(Vertex v : vert)
					{
						vertices.push_back(v);
					}					
			   }
			   this->maxParticles = maxParticles;
			   Finalize();
		   }

		   void Update(const float dt)
		   {
			   for(Particle* p : particles)
			   {
				   p->Update(dt);
			   }
		   }

private: 
	void Finalize()
		 {
			 //Dynamic vertex Buffer
				 D3D11_BUFFER_DESC			desc;
				 desc.Usage					= D3D11_USAGE_DYNAMIC;
				 desc.CPUAccessFlags		= D3D11_CPU_ACCESS_WRITE;
				 desc.ByteWidth				= sizeof(Vertex) * maxParticles * 4;
				 desc.BindFlags				= D3D11_BIND_VERTEX_BUFFER;
				 desc.MiscFlags				= 0;
				 desc.StructureByteStride	= 0;

				 D3D11_SUBRESOURCE_DATA initialVertexData;
				 initialVertexData.pSysMem = &vertices.at(0);

				  HR(DXConnection::Instance()->device->CreateBuffer(
					&desc,
					&initialVertexData,
					&buffer));

			  //Normal Index Buffer
			  	D3D11_BUFFER_DESC ibd;
				ibd.Usage					= D3D11_USAGE_IMMUTABLE;
				ibd.ByteWidth				= sizeof(UINT) * maxParticles; // Number of indices
				ibd.BindFlags				= D3D11_BIND_INDEX_BUFFER;
				ibd.CPUAccessFlags			= 0;
				ibd.MiscFlags				= 0;
				ibd.StructureByteStride		= 0;

				D3D11_SUBRESOURCE_DATA initialIndexData;
				initialIndexData.pSysMem	= &indices.at(0);

				HR(DXConnection::Instance()->device->CreateBuffer(
					&ibd,
					&initialIndexData,
					&indexBuffer));


	}
		 vector<UINT> indices;
		 vector<Vertex> vertices;
		 ID3D11Buffer* indexBuffer;
		 ID3D11Buffer* buffer;
		 long maxParticles;
};