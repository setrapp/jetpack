#define __PATH "content"
#include "Material.h"
#include "Toolkit\Inc\WICTextureLoader.h"
#include "Toolkit\Inc\DDSTextureLoader.h"
#include "Common.h"
#include <string>
#include <wchar.h>
#include "AssetManager.h"

using namespace DirectX;

Material::Material()
{
	Init();
}

Material::Material(XMFLOAT4 ambient, XMFLOAT4 diffuse, float specular, UINT shininess)
{	
	Init();
	this->ambient = ambient;
	this->diffuse = diffuse;
	this->specular = specular;
	this->shininess = shininess;
}

Material::Material(wchar_t* path)
{	
	Init();
	ApplyTexture(path);
}

void Material::Init()
{
	vertexShader = AssetManager::Instance()->GetVertexShader();
	pixelShader = AssetManager::Instance()->GetPixelShader();
	this->ambient = XMFLOAT4(0.3, 0.3f, 0.3f, 1);
	this->diffuse = XMFLOAT4(0.3, 0.3f, 0.3f, 1);
	this->specular = 0;
	this->shininess = 0;
}

Material::~Material(void)
{
	// TODO This stuff should probably be owned by AssetManager. Not trivial to port though. //
	ReleaseMacro(texture);
	ReleaseMacro(resourceView);
	ReleaseMacro(samplerState);
}

void Material::ApplyTexture(wchar_t* path, bool isDDS)
{
	//load the texture
	if(!isDDS)
	{
	ID3D11Resource* tex = texture;
	HR (CreateWICTextureFromFile(
		DXConnection::Instance()->device, 
		DXConnection::Instance()->deviceContext, 
		path, 
		&texture, 
		&this->resourceView));
	}
	else
	{
		HR (CreateDDSTextureFromFile(
		DXConnection::Instance()->device, 
		path, 
		&texture, 
		&this->resourceView));
	}

	//create the sampler.
	D3D11_SAMPLER_DESC sBufferDesc;
	sBufferDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sBufferDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sBufferDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sBufferDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	sBufferDesc.MaxAnisotropy = 64;
	sBufferDesc.MipLODBias = 64;

	DXConnection::Instance()->device->CreateSamplerState(
		&sBufferDesc,
		&this->samplerState);
}



ShaderMaterial Material::GetShaderMaterial()
{
	ShaderMaterial shaderMat;
	shaderMat.ambient = ambient;
	shaderMat.diffuse = diffuse;
	shaderMat.extras = XMFLOAT4(specular, shininess / 256.0f, 0, 1.0f);
	return shaderMat;
}

