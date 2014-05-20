#pragma once 
#define WIN32_LEAN_AND_MEAN
#include "Entity.h"
#include "Toolkit\Inc\WICTextureLoader.h"
#include <Windows.h>
#include <d3dcompiler.h>
#include "Common.h"
#include <stdio.h>
#include <stdlib.h>
#include <ostream>
#include <string>
#include <wchar.h>
#include "AssetManager.h"
using namespace DirectX;

Entity::Entity()
{
	baseMaterial = AssetManager::Instance()->GetMaterial();
	socketNumber=0;
	visible = true;
	transform.entity = this;
}


Entity::~Entity(void)
{
	for (vector<Mesh*>::iterator it = meshes.begin(); it != meshes.end(); it++)
	{
		delete *it;
	}
	ReleaseMacro(vertexBuffer);
	for (map<Material*, pair<ID3D11Buffer*, LONG>>::iterator it = indexBuffers.begin(); it != indexBuffers.end(); it++)
	{
		ReleaseMacro(it->second.first);
	}
}

void Entity::AddTriangle(Vertex* v, UINT* i)
{
	for (int j = 0; j < 3; j++)
	{
		i[j] += vertices.size();
	}

	for(int i = 0; i < 3; i++)
		vertices.push_back(v[i]);
	Mesh* m = new Mesh(i);
	meshes.push_back(m);
}

void Entity::AddQuad(Vertex* v, UINT* i)
{
	for (int j = 0; j < 6; j++)
	{
		i[j] += vertices.size();
	}

	for(int i = 0; i < 4; i++)
		vertices.push_back(v[i]);
	Mesh* m1 = new Mesh(i);
	Mesh* m2 = new Mesh(i + 3);
	meshes.push_back(m1);
	meshes.push_back(m2);
}

void Entity::AddMeshGroup(Model* sourceModel, MeshGroup* meshGroup, bool removeFacesFromModel)
{
	int existingVertexCount = vertices.size();
	int vertexCount = sourceModel->vertices.size();
	int meshCount = sourceModel->meshes.size();
	vector<UINT*> meshGroupIndices;

	// Extract relevant faces.
	for (int i = meshGroup->firstFace; i <= meshGroup->lastFace && i < meshCount; i++)
	{
		UINT* indices = sourceModel->meshes[i].GetIndices();
		for (int j = 0; j < 3; j++)
		{
			indices[j] += existingVertexCount;
		}
		meshGroupIndices.push_back(indices);
		Mesh* newMesh = new Mesh(indices);
		newMesh->SetMaterial(sourceModel->meshes[i].GetMaterial());
		meshes.push_back(newMesh);
	}
	
	// Extract relavant vertices and point face indices at them.
	int meshGroupIndexCount = meshGroupIndices.size();
	for (int i = 0; i < vertexCount; i++)
	{
		bool vertexFound = false;
		for (int j = 0; j < meshGroupIndexCount; j++)
		{
			for (int k = 0; k < 3; k++)
			{
				if (meshGroupIndices[j][k] == i)
				{
					if (!vertexFound)
					{
						vertices.push_back(sourceModel->vertices[i]);
						vertexFound = true;
					}
					meshes[j]->GetIndices()[k] = vertices.size() - 1;
				}
			}
		}
	}

	// If specified, set faces on the source model to NULL so they will be ignored.
	if (removeFacesFromModel)
	{
		int indexShift = (meshGroup->lastFace - meshGroup->firstFace) + 1;
		for (int i = 0; i < sourceModel->meshGroups.size(); i++)
		{
			if(sourceModel->meshGroups[i].firstFace > meshGroup->lastFace)
			{
				
				sourceModel->meshGroups[i].firstFace -= indexShift;
				sourceModel->meshGroups[i].lastFace -= indexShift;
			}
		}
		for (int i = meshGroup->firstFace; i <= meshGroup->lastFace && i < meshCount; meshGroup->lastFace--)
		{
			sourceModel->meshes.erase(sourceModel->meshes.begin() + i);
		}
	}
}

void Entity::AddModel(Model* model) {
	if (!model)
	{
		return;
	}

	int oldVertexCount = vertices.size();
	for (int i = 0; i < model->vertices.size(); i++)
	{
		vertices.push_back(model->vertices[i]);
	}
	for (int i = 0; i < model->meshes.size(); i++)
	{
		UINT* modelIndices = model->meshes[i].GetIndices();
		UINT indices[3];
		for (int j = 0; j < 3; j++)
		{
			indices[j] = modelIndices[j] + oldVertexCount;
		}
		Mesh* mesh = new Mesh(indices);
		mesh->SetMaterial(model->meshes[i].GetMaterial());
		meshes.push_back(mesh);
	}

	models.push_back(model);
}

void Entity::RecenterGeometry()
{
	transform.SetTranslation(XMFLOAT3(0, 0, 0));
	int vertexCount = vertices.size();
	XMFLOAT3 centroid = XMFLOAT3(0, 0, 0);
	for (int i = 0; i < vertexCount; i++)
	{
		centroid.x += vertices[i].Position.x;
		centroid.y += vertices[i].Position.y;
		centroid.z += vertices[i].Position.z;
	}
	centroid.x /= vertexCount;
	centroid.y /= vertexCount;
	centroid.z /= vertexCount;
	for (int i = 0; i < vertexCount; i++)
	{
		vertices[i].Position.x -= centroid.x;
		vertices[i].Position.y -= centroid.y;
		vertices[i].Position.z -= centroid.z;
	}
	XMFLOAT3 translation;
	XMStoreFloat3(&translation, XMVectorMultiply(XMLoadFloat3(&centroid), XMLoadFloat3(&transform.GetScale())));
	transform.SetTranslation(translation);
}

XMFLOAT3 Entity::ProjectPointOnMesh(Mesh* mesh, XMFLOAT3 point, bool pointInEntitySpace, XMFLOAT3* normalOut)
{
	UINT* meshIndices = mesh->GetIndices();
	Vertex meshVertices[] = {vertices[meshIndices[0]], vertices[meshIndices[1]], vertices[meshIndices[2]]};
	XMFLOAT3 faceNormal;
	XMStoreFloat3(&faceNormal, XMVector3Normalize(XMVector3Cross(
			XMVectorSubtract(XMLoadFloat3(&meshVertices[1].Position), XMLoadFloat3(&meshVertices[0].Position)), 
			XMVectorSubtract(XMLoadFloat3(&meshVertices[2].Position), XMLoadFloat3(&meshVertices[0].Position)))));
	
	// Ensure that point is in entity space.
	if (!pointInEntitySpace)
	{
		point = transform.InverseTransformPoint(point);
	}

	// Find nearest point on mesh to given point.
	XMFLOAT3 vertToPoint;
	XMStoreFloat3(&vertToPoint, XMVectorSubtract(XMLoadFloat3(&point), XMLoadFloat3(&meshVertices[1].Position)));
	float toDotNorm;
	XMStoreFloat(&toDotNorm, XMVector3Dot(XMLoadFloat3(&vertToPoint), XMLoadFloat3(&faceNormal)));
	XMFLOAT3 meshPoint;
	XMStoreFloat3(&meshPoint, XMVectorSubtract(XMLoadFloat3(&point), XMVectorScale(XMLoadFloat3(&faceNormal), toDotNorm)));

	// Ensure that returned point is in the correct space.
	if (!pointInEntitySpace)
	{
		meshPoint = transform.TransformPoint(meshPoint);
	}
	return meshPoint;
}

void Entity::Update(float dt)
{
	
}

void Entity::Draw(EntityDrawArgs const* drawArgs, XMFLOAT4X4 const* view, XMFLOAT4X4 const* projection)
{
	if (!visible || !drawArgs)
	{
		return;
	}

	// Prepare vertex buffer.
	const UINT stride = sizeof(Vertex);
	UINT offset = 0;
	DXConnection::Instance()->deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

	// Compute the inverse transpose of the entity's world matrix for use by normals in the shaders. Ignore translation.
	// If the entity is scaled uniformly, cheat and use the world matrix because scales will work.
	XMFLOAT3X3 rotationScale;
	XMStoreFloat3x3(&rotationScale, XMLoadFloat4x4(&transform.GetWorldMatrix()));
	XMFLOAT4X4 inverseTranspose;
	if (transform.IsUniformScale())
	{
		XMStoreFloat4x4(&inverseTranspose, XMLoadFloat3x3(&rotationScale));
	}
	else
	{
		XMStoreFloat4x4(&inverseTranspose, XMMatrixTranspose(XMMatrixInverse(nullptr, XMLoadFloat3x3(&rotationScale))));
	}

	// Create per primitive vertex shader constant buffer to hold matrices.
	VertexShaderModelConstantBuffer perPrimitiveVSConstantBuffer;
	perPrimitiveVSConstantBuffer.world = transform.GetWorldMatrix();
	perPrimitiveVSConstantBuffer.inverseTranspose = inverseTranspose;
	if (view)
	{
		perPrimitiveVSConstantBuffer.view = *view;
	}
	else
	{
		perPrimitiveVSConstantBuffer.view = drawArgs->vsModelConstantBufferData->view;
	}
	if (projection)
	{
		perPrimitiveVSConstantBuffer.projection = *projection;
	}
	else
	{
		perPrimitiveVSConstantBuffer.projection = drawArgs->vsModelConstantBufferData->projection;
	}

	// Update vertex shader constant buffer with per primitive buffer.
	DXConnection::Instance()->deviceContext->UpdateSubresource(drawArgs->vsModelConstantBuffer, 0, nullptr, &perPrimitiveVSConstantBuffer, 0, 0);

	// Prepare index buffers categorized by material.
	for (map<Material*, pair<ID3D11Buffer*, LONG>>::iterator it = indexBuffers.begin(); it != indexBuffers.end(); it++)
	{
		Material* useMaterial = it->first;
		if (!useMaterial)
		{
			useMaterial = AssetManager::Instance()->GetMaterial();
		}

		// Create per primitive pixel shader constant buffer to hold materials.
		MaterialsAndLightsConstantBuffer perPrimitiveMaterialConstantBuffer;
		perPrimitiveMaterialConstantBuffer.light = drawArgs->materialsAndLightsConstantBufferData->light;
		perPrimitiveMaterialConstantBuffer.material = useMaterial->GetShaderMaterial();

		// Update pixel shader constant buffer with per primitive materials buffer.
		DXConnection::Instance()->deviceContext->UpdateSubresource(drawArgs->materialsAndLightsConstantBuffer, 0, nullptr, &perPrimitiveMaterialConstantBuffer, 0, 0);

		ID3D11DeviceContext* deviceContext = DXConnection::Instance()->deviceContext;
		deviceContext->VSSetShader(useMaterial->vertexShader, NULL, 0);
		deviceContext->PSSetShader(useMaterial->pixelShader, NULL, 0);
		if (useMaterial->resourceView)
		{
			deviceContext->PSSetShaderResources(0, 1, &useMaterial->resourceView);
		}		
		if (useMaterial->samplerState)
		{
			deviceContext->PSSetSamplers(0, 1, &useMaterial->samplerState);
		}

		DXConnection::Instance()->deviceContext->IASetIndexBuffer(it->second.first, DXGI_FORMAT_R32_UINT, 0);

		DXConnection::Instance()->deviceContext->DrawIndexed(
		it->second.second,	// The number of indices we're using in this draw
		0,
		0);
	}	
}


void Entity::LoadTexture(wchar_t* path, bool isDDS)
{	
	this->baseMaterial->ApplyTexture(path, isDDS);
}


Material* Entity::GetBaseMaterial()
{
	return this->baseMaterial;
}


inline Material Entity::GetBaseMaterialSafe() const
{
	return *this->baseMaterial;
}

void Entity::SetBaseMaterial(string name, Model* sourceModel, bool forceOnAllMeshes)
{
	Material* newBaseMaterial = AssetManager::Instance()->GetMaterial(name, sourceModel);
	Material* defaultMaterial = AssetManager::Instance()->GetMaterial();
	for (vector<Mesh*>::iterator it = meshes.begin(); it != meshes.end(); it++)
	{
		Material* meshMaterial = (*it)->GetMaterial();
		if (forceOnAllMeshes || meshMaterial == baseMaterial || meshMaterial == defaultMaterial)
		{
			(*it)->SetMaterial(newBaseMaterial);
		}
	}
	baseMaterial = newBaseMaterial;
}

inline vector<Mesh*> Entity::GetMeshes() const 
{
	return meshes;
}

void Entity::Finalize()
{
	if (meshes.size() < 1 || vertices.size() < 1)
	{
		return;
	}

	map<Material*, vector<UINT>*> indicesAll;
	long totalMeshes = meshes.size();
	for(int i = 0; i < totalMeshes; i++)
	{
		Material* meshMaterial = meshes[i]->GetMaterial();
		if (meshMaterial)
		{
			map<Material*, vector<UINT>*>::iterator matIt = indicesAll.find(meshMaterial);
			vector<UINT>* materialIndices;
			if (matIt != indicesAll.end())
			{
				materialIndices = matIt->second;
			}
			else
			{
				materialIndices = new vector<UINT>;
				indicesAll.insert(pair<Material*, vector<UINT>*>(meshMaterial, materialIndices));
			}
			UINT* indices = meshes.at(i)->GetIndices();
			for(short j= 0; j < 3; j++)
			{
				materialIndices->push_back(indices[j]);
			}
		}
	}

	for (map<Material*, vector<UINT>*>::iterator it = indicesAll.begin(); it != indicesAll.end(); it++)
	{
		vector<UINT>* materialIndices = it->second;
		long size = materialIndices->size();
		UINT* indexPtr = &materialIndices->at(0);

		//// Create the index buffer
		D3D11_BUFFER_DESC ibd;
		ibd.Usage					= D3D11_USAGE_IMMUTABLE;
		ibd.ByteWidth				= sizeof(UINT) * size; // Number of indices
		ibd.BindFlags				= D3D11_BIND_INDEX_BUFFER;
		ibd.CPUAccessFlags			= 0;
		ibd.MiscFlags				= 0;
		ibd.StructureByteStride		= 0;

		D3D11_SUBRESOURCE_DATA initialIndexData;
		initialIndexData.pSysMem	= indexPtr;

		ID3D11Buffer* indexBuffer;
		HR(DXConnection::Instance()->device->CreateBuffer(
			&ibd,
			&initialIndexData,
			&indexBuffer));
		indexBuffers.insert(pair<Material*, pair<ID3D11Buffer*, LONG>>(it->first, pair<ID3D11Buffer*, LONG>(indexBuffer, size)));
	}

	D3D11_BUFFER_DESC vbd;
    vbd.Usage					= D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth				= sizeof(Vertex) * vertices.size(); // Number of vertices
    vbd.BindFlags				= D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags			= 0;
    vbd.MiscFlags				= 0;
	vbd.StructureByteStride		= 0;

    D3D11_SUBRESOURCE_DATA initialVertexData;
    initialVertexData.pSysMem	= &vertices.at(0);

    HR(DXConnection::Instance()->device->CreateBuffer(
		&vbd,
		&initialVertexData,
		&vertexBuffer));

	for(map<Material*, vector<UINT>*>::iterator it = indicesAll.begin(); it != indicesAll.end(); it++)
	{
		delete it->second;
	}
}


bool Entity::GetVisible()
{
	return visible;
}

void Entity::SetVisible(bool visibility)
{
	visible = visibility;

	Transform* parent = transform.GetParent() ;
	if (visibility = true && parent && parent->entity && !transform.GetParent()->entity->visible)
	{
		visibility = false;
	}

	vector<Transform*>* children = &transform.children;
	for(vector<Transform*>::iterator it = children->begin(); it != children->end(); it++)
	{
		if((*it)->entity)
		{
			(*it)->entity->SetVisible(visibility);
		}
	}
}

int Entity::GetVertexCount()
{
	return vertices.size();
}

Vertex* Entity::GetVertex(int index)
{
	if (index > vertices.size())
	{
		return NULL;
	}
	return &vertices[index];
}

int Entity::GetMeshCount()
{
	return meshes.size();
}

Mesh* Entity::GetMesh(int index)
{
	if (index > meshes.size())
	{
		return NULL;
	}
	return meshes[index];
}

int Entity::GetModelCount()
{
	return models.size();
}

Model* Entity::GetModel(int index)
{
	if (index > models.size())
	{
		return NULL;
	}
	return models[index];
}

string Entity::getNetworkString(){
	XMFLOAT3 curTransform= transform.GetLocalTranslation();
	XMFLOAT3X3 curRotation = transform.GetLocalRotation();
	std::ostringstream ss1;
	ss1 << curTransform.x;
	ss1 << ",";
	ss1 << curTransform.y;
	ss1 << ",";
	ss1 << curTransform.z;
	ss1 << ",";
	ss1 << curRotation._11;
	ss1 << ",";
	ss1 << curRotation._12;
	ss1 << ",";
	ss1 << curRotation._13;
	ss1 << ",";
	ss1 << curRotation._21;
	ss1 << ",";
	ss1 << curRotation._22;
	ss1 << ",";
	ss1 << curRotation._23;
	ss1 << ",";
	ss1 << curRotation._31;
	ss1 << ",";
	ss1 << curRotation._32;
	ss1 << ",";
	ss1 << curRotation._33;
	string s1= ss1.str();
	
	return s1;
}
