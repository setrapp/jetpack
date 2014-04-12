#define __PATH "content"
#include "Material.h"
#include "Toolkit\Inc\WICTextureLoader.h"
#include "Common.h"
#include <string>
#include <wchar.h>

using namespace DirectX;

Material::Material()
{
	this->ambient = XMFLOAT4(0.3, 0.3f, 0.3f, 1);
	this->diffuse = XMFLOAT4(0.3, 0.3f, 0.3f, 1);
	this->specular = XMFLOAT4(0, 0, 0, 1);
	this->shininess = 0;
}

Material::Material(XMFLOAT4 ambient, XMFLOAT4 diffuse, XMFLOAT4 specular, UINT shininess)
{	
	this->ambient = ambient;
	this->diffuse = diffuse;
	this->specular = specular;
	this->shininess = shininess;
}

Material::Material(wchar_t* path)
{	
	this->ambient = XMFLOAT4(0.3, 0.3f, 0.3f, 1);
	this->diffuse = XMFLOAT4(0.3, 0.3f, 0.3f, 1);
	this->specular = XMFLOAT4(0, 0, 0, 1);
	this->shininess = 0;
	ApplyTexture(path);
}

Material::~Material(void)
{
}

void Material::ApplyTexture(wchar_t* path)
{
	ID3D11Resource* tex = texture;
	HR (CreateWICTextureFromFile(
		DXConnection::Instance()->device, 
		DXConnection::Instance()->deviceContext, 
		path, 
		&texture, 
		&this->resourceView));

	D3D11_SAMPLER_DESC sBufferDesc;
	sBufferDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sBufferDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sBufferDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sBufferDesc.Filter = D3D11_FILTER_COMPARISON_ANISOTROPIC;
	sBufferDesc.MaxAnisotropy = 16;

	DXConnection::Instance()->device->CreateSamplerState(
		&sBufferDesc,
		&this->samplerState);
}

ShaderMaterial Material::GetShaderMaterial()
{
	ShaderMaterial shaderMat;
	shaderMat.ambient = ambient;
	shaderMat.diffuse = diffuse;
	shaderMat.specular = specular;
	shaderMat.shininess = XMUINT4(shininess, shininess, shininess, shininess);
	return shaderMat;
}

