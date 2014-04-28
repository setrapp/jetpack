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

struct EntityDrawArgs
{
	ID3D11Buffer* vsModelConstantBuffer;
	VertexShaderModelConstantBuffer* vsModelConstantBufferData;
	ID3D11Buffer* materialsAndLightsConstantBuffer;
	MaterialsAndLightsConstantBuffer* materialsAndLightsConstantBufferData;
	
};

class Entity
{
public:
	Entity();
	virtual ~Entity();
	void AddTriangle(Vertex* v, UINT* u);
	void AddQuad(Vertex* v, UINT* u);
	void AddModel(Model* model);
	virtual void Update(float dt);
	void Draw(EntityDrawArgs const* drawArgs);
	void LoadTexture(wchar_t* path);
	Material* GetBaseMaterial();
	inline Material GetBaseMaterialSafe() const;
	void SetBaseMaterial(string name = "default", bool forceOnAllMeshes = true);
	inline vector<Mesh*> GetMeshes() const;
	string getNetworkString();
	Transform transform;
	int socketNumber;
	void Finalize();

private:
	map<Material*, pair<ID3D11Buffer*, LONG>> indexBuffers;
	ID3D11Buffer* vertexBuffer;
	vector<Vertex> vertices;
	vector<Mesh*> meshes; 
	Material* baseMaterial;
};

#endif