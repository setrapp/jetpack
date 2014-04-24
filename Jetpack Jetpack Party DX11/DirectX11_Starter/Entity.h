#ifndef ENTITY_H
#define ENTITY_H
#pragma once
#define WIN32_LEAN_AND_MEAN
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
	DirectX::XMFLOAT4X4 GetWorldMatrix() const;
	Material* GetMaterial();
	inline Material GetMaterialSafe() const;
	void SetMaterial(string name = "default");
	inline vector<Mesh*> GetMeshes() const;
	Transform transform;
	int socketNumber;
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

#endif