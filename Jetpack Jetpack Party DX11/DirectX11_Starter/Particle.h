#pragma once
#include "Common.h"
#include "AssetManager.h"
#include "Transform.h"
#include "Material.h"
#include <DirectXMath.h>
#include "DXConnection.h"

using namespace std;
using namespace DirectX;

class Particle
{
public: 
	Transform transform;
	Material* material;
	Particle()
	{
		Vertex v[4] = {
		{ XMFLOAT3(+1.0f, +1.0f, +0.0f), XMFLOAT3(0, 0, -1), XMFLOAT2(0, 0) },
		{ XMFLOAT3(-1.0f, -1.0f, +0.0f), XMFLOAT3(0, 0, -1), XMFLOAT2(1, 1) },
		{ XMFLOAT3(+1.0f, -1.0f, +0.0f), XMFLOAT3(0, 0, -1), XMFLOAT2(0, 1) },		
		{ XMFLOAT3(-1.0f, +1.0f, +0.0f), XMFLOAT3(0, 0, -1), XMFLOAT2(1, 0) },
	};

	UINT indices[] = { 0, 2, 1, 3, 0, 1 };
		D3D11_BUFFER_DESC ibd;
		ibd.Usage					= D3D11_USAGE_IMMUTABLE;
		ibd.ByteWidth				= sizeof(UINT) * 6; // Number of indices
		ibd.BindFlags				= D3D11_BIND_INDEX_BUFFER;
		ibd.CPUAccessFlags			= 0;
		ibd.MiscFlags				= 0;
		ibd.StructureByteStride		= 0;

		D3D11_SUBRESOURCE_DATA initialIndexData;
		initialIndexData.pSysMem	= indices;

		ID3D11Buffer* indexBuffer;
		HR(DXConnection::Instance()->device->CreateBuffer(
			&ibd,
			&initialIndexData,
			&indexBuffer));

		XMStoreFloat4x4(&trans, XMMatrixIdentity());
		XMStoreFloat4x4(&scale, XMMatrixIdentity());
		XMStoreFloat4x4(&rot, XMMatrixIdentity());
		D3D11_BUFFER_DESC vbd;
		vbd.Usage					= D3D11_USAGE_IMMUTABLE;
		vbd.ByteWidth				= sizeof(Vertex) * 4; // Number of vertices
		vbd.BindFlags				= D3D11_BIND_VERTEX_BUFFER;
		vbd.CPUAccessFlags			= 0;
		vbd.MiscFlags				= 0;
		vbd.StructureByteStride		= 0;

		D3D11_SUBRESOURCE_DATA initialVertexData;
		initialVertexData.pSysMem	= v;

		HR(DXConnection::Instance()->device->CreateBuffer(
			&vbd,
			&initialVertexData,
			&vertexBuffer));

		material = AssetManager::Instance()->GetMaterial("gift");
		material->pixelShader =  AssetManager::Instance()->GetPixelShader("texture");
	}
	
void Draw(EntityDrawArgs const* drawArgs, XMFLOAT4X4 const* view = NULL, XMFLOAT4X4 const* projection = NULL)
{

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
	
	Material* useMaterial = material;
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

		DXConnection::Instance()->deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

		DXConnection::Instance()->deviceContext->DrawIndexed(
		6,	// The number of indices we're using in this draw
		0,
		0);
	}	

	XMFLOAT4X4 GetWorldMatrix()
	{
		return transform.GetWorldMatrix();
	}
	
private:
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	XMFLOAT4X4 trans, rot, scale;
};