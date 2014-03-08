#pragma once
#include "Vertex.h"
#include "Common.h"
#include <vector>
#include "Mesh.h"
#include "Material.h"
#include "Transform.h"
#include "Rigidbody.h"


class Entity
{
public:
	Entity(ID3D11Device* device);
	~Entity(void);
	void AddTraingle(Vertex* v, UINT* u);
	void AddQuad(Vertex* v, UINT* u);
	void Update(float dt, VertexShaderConstantBuffer* vsConstantBuffer);
	void Draw(ID3D11DeviceContext* deviceContext);	
	void Rotate(XMFLOAT3 rotation);
	void Translate(XMFLOAT3 translation);
	void Scale(XMFLOAT3 scale);	
	void LoadTexture(wchar_t* path, ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	DirectX::XMFLOAT4X4 GetWorldMatrix();
	Material* GetMaterial();
	Material GetMaterialSafe();
	std::vector<Mesh*> GetMeshes();
	Transform* transform;

private:
	ID3D11Device* device;
	LONG totalMeshes;				//keep this. 
	std::vector<Mesh*> meshes; 
	Material* material;
};

