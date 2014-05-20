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
	void AddMeshGroup(Model* sourceModel, MeshGroup* meshGroup, bool removeFacesFromModel = false);
	void AddModel(Model* model);
	void RecenterGeometry();
	XMFLOAT3 ProjectPointOnMesh(Mesh* mesh, XMFLOAT3 point, bool pointInEntitySpace = false, XMFLOAT3* normalOut = NULL);
	virtual void Update(float dt);
	void Draw(EntityDrawArgs const* drawArgs, XMFLOAT4X4 const* view = NULL, XMFLOAT4X4 const* projection = NULL);
	void LoadTexture(wchar_t* path, bool isDDS = false);
	Material* GetBaseMaterial();
	inline Material GetBaseMaterialSafe() const;
	void SetBaseMaterial(string name = "default", Model* sourceModel = NULL, bool forceOnAllMeshes = true);
	inline vector<Mesh*> GetMeshes() const;
	string getNetworkString();
	Transform transform;
	int socketNumber;
	virtual void Finalize();
	bool GetVisible();
	void SetVisible(bool visibility);
	int GetVertexCount(); 
	Vertex* GetVertex(int index); 
	int GetMeshCount(); 
	Mesh* GetMesh(int index);
	int GetModelCount(); 
	Model* GetModel(int index); 

	//virtual void Update(float dt);
	//virtual void Draw(EntityDrawArgs const* drawArgs, XMFLOAT4X4 const* view = NULL, XMFLOAT4X4 const* projection = NULL);

protected:
	map<Material*, pair<ID3D11Buffer*, LONG>> indexBuffers;
	ID3D11Buffer* vertexBuffer;
	vector<Vertex> vertices;
	vector<Mesh*> meshes; 
	vector<Model*> models;
	Material* baseMaterial;
	bool visible;
};

#endif
