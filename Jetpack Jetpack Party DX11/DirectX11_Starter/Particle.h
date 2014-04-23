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
	Transform* transform;

	Particle(const string materialName, XMFLOAT3 position, XMFLOAT2 size, XMFLOAT4 color, float age, XMFLOAT3 velocity)
	{
		material = AssetManager::Instance()->GetMaterial(materialName);
		this->age= age;
		this->color = color;
		this->size = size;
		this->position = position;
		this->velocity = velocity;
	}

	void Particle::Update()
	{
		
	}

};


typedef struct ParticleConstantBuffer
{
	XMFLOAT3 position;
};