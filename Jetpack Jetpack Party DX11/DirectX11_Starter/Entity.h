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
	void LoadTexture(wchar_t* path);
	DirectX::XMFLOAT4X4 GetWorldMatrix();
	Material* GetMaterial();
	Material GetMaterialSafe();
	void SetMaterial(string name = "default");
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

