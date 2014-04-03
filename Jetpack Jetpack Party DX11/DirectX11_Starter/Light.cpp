#include "Light.h"

Light::Light()
{
	transform = new Transform();
	this->color = XMFLOAT4(1, 1, 1, 1);
	this->isDirectional = false;
}

Light::Light(XMFLOAT3 position, XMFLOAT4 color, bool isDirectional)
{
	transform = new Transform();
	transform->Translate(position);
	this->color = color;
	this->isDirectional = isDirectional;
}

Light Light::GetUsableCopy()
{
	// Create copy of light and store its state as a point or directional light in the world matrix;
	Light light = *this;
	if (!isDirectional)
	{
		light.transform->worldMatrix._44 = 1.0f;
	} 
	else 
	{
		light.transform->worldMatrix._44 = 0.0f;
	}

	return light;
}