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
	Entity();
	~Entity(void);
	void AddTriangle(Vertex* v, UINT* u);
	void AddQuad(Vertex* v, UINT* u);
	virtual void Update(float dt);
	void Draw();	
	void Rotate(XMFLOAT3 rotation);
	void Translate(XMFLOAT3 translation);
	void Scale(XMFLOAT3 scale);	
	void LoadTexture(wchar_t* path);
	DirectX::XMFLOAT4X4 GetWorldMatrix();
	Material* GetMaterial();
	Material GetMaterialSafe();
	std::vector<Mesh*> GetMeshes();
	Transform* transform;

private:
	LONG totalMeshes;				//keep this. 
	std::vector<Mesh*> meshes; 

	Material* material;
};

