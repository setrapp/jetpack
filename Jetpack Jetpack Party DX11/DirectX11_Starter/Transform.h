#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

#include <DirectXMath.h>
#include <d3d11.h>

class Transform
{
public :
	DirectX::XMFLOAT4X4 trans, rot, scale;
	DirectX::XMFLOAT4X4 worldMatrix;
	BOOL changed;

	Transform::Transform()
	{
		changed = false;
	}

	Transform::~Transform()
	{

	}

	void Transform::Rotate(XMFLOAT3 rotation)
	{
		changed = true;
		XMStoreFloat4x4(&this->rot,  XMMatrixMultiply(XMLoadFloat4x4(&this->rot),XMMatrixRotationZ(rotation.z) * XMMatrixRotationY(rotation.y) *  XMMatrixRotationX(rotation.x)));
	 }

	void Transform::Translate(XMFLOAT3 translation)
	{
		changed = true;
		XMStoreFloat4x4(&this->trans, XMMatrixMultiply(XMLoadFloat4x4(&this->trans), XMMatrixTranslation(translation.x, translation.y, translation.z)));
	}

	void Transform::Scale(XMFLOAT3 scale)
	{
		changed = true;
		XMStoreFloat4x4(&this->scale, XMMatrixMultiply(XMLoadFloat4x4(&this->scale), XMMatrixScaling(scale.x, scale.y, scale.z)));
	}
};
#endif