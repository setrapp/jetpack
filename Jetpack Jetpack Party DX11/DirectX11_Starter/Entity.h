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
	void AddModel(Model* model);
	virtual void Update(float dt);
	void Draw();
	void Rotate(XMFLOAT3 rotation);
	void Translate(XMFLOAT3 translation);
	void Scale(XMFLOAT3 scale);	
	void LoadTexture(wchar_t* path);
	DirectX::XMFLOAT4X4 GetWorldMatrix();
	Material* GetMaterial();
	Material GetMaterialSafe();
	vector<Mesh*> GetMeshes();
	Transform* transform;
	void Finalize();

private:
	ID3D11Buffer* indexBuffer;
	ID3D11Buffer* vertexBuffer;
	vector<Vertex> vertices;
	LONG totalMeshes;			 
	vector<Mesh*> meshes; 
	Material* material;
	LONG totalIndices;
};

