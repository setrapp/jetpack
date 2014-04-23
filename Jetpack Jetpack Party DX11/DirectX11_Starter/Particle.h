#pragma once 
#include <d3d11.h>
#include "Common.h"
#include "DXConnection.h"
#include <DirectXMath.h>
#include "Material.h"
#include "AssetManager.h"
#include "Transform.h"

using namespace DirectX;

class Particle {
	XMFLOAT3 position;
	XMFLOAT2 size;
	XMFLOAT4 color;
	float age;
	XMFLOAT3 velocity;
	Material* material;
	bool enabled;
	float life;

	UINT* indices;
	Vertex* vertices;

public:
	
	Transform* transform;
	Particle(const string materialName, XMFLOAT3 position, XMFLOAT2 size, XMFLOAT4 color, float age, XMFLOAT3 velocity)
	{
		material = AssetManager::Instance()->GetMaterial(materialName);
		this->age= age;
		this->color = color;
		this->size = size;
		this->position = position;
		this->velocity = velocity;
		enabled = true;
		this->transform = new Transform();
		life = 0;
	}

	void Particle::Update(const float dt)
	{
		position = this->transform->GetTranslation();
		life += dt;
		if(life > age)
		{
			enabled = false;
		}
	}

	void Render()
	{
		//DXConnection::Instance()->deviceContext-
	}

};


typedef struct ParticleConstantBuffer
{
	XMFLOAT3 position;
};
