#define __PATH "content"
#include "Material.h"
#include "Toolkit\Inc\WICTextureLoader.h"
#include "Common.h"
#include <string>
#include <wchar.h>

using namespace DirectX;

Material::Material(ID3D11Device* device, ID3D11DeviceContext* deviceContext, wchar_t* path)
{	
	HR (CreateWICTextureFromFile(
		device, 
		deviceContext, 
		path, 
		&texture, 
		&this->resourceView));

	D3D11_SAMPLER_DESC sBufferDesc;
	sBufferDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sBufferDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sBufferDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sBufferDesc.Filter = D3D11_FILTER_COMPARISON_ANISOTROPIC;
	sBufferDesc.MaxAnisotropy = 16;

	device->CreateSamplerState(
		&sBufferDesc,
		&this->samplerState);
}


Material::~Material(void)
{
}

