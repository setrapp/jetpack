#include "Light.h"

Light::Light()
{
	transform = new Transform();
	this->ambient = XMFLOAT4(0, 0, 0, 1);
	this->diffuse = XMFLOAT4(1, 1, 1, 1);
	this->specular = XMFLOAT4(1, 1, 1, 1);
	this->isDirectional = false;
}

Light::Light(XMFLOAT3 position, XMFLOAT4 ambient, XMFLOAT4 diffuse, XMFLOAT4 specular, bool isDirectional)
{
	transform = new Transform();
	transform->Translate(position);
	this->ambient = ambient;
	this->diffuse = diffuse;
	this->specular = specular;
	this->isDirectional = isDirectional;
}

ShaderLight Light::ConvertToShaderLight()
{
	// Create copy of light and store its state as a point or directional light in the world matrix;
	ShaderLight shaderLight;
	
	// TODO World Matrix should update properly, should light extend entity?
	//XMStoreFloat4x4(&this->transform->worldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&transform->scale) * XMLoadFloat4x4(&transform->rot) * XMLoadFloat4x4(&transform->trans)));

	shaderLight.world = transform->GetWorldMatrix();
	if (!isDirectional)
	{
		shaderLight.world._44 = 1.0f;
	} 
	else 
	{
		shaderLight.world._44 = 0.0f;
	}
	shaderLight.ambient = ambient;
	shaderLight.diffuse = diffuse;
	shaderLight.specular = specular;

	return shaderLight;
}