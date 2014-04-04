#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

#include <DirectXMath.h>
#include <d3d11.h>

using namespace DirectX;

class Transform
{
public :
	DirectX::XMFLOAT4X4 trans, rot, scale;
	DirectX::XMFLOAT4X4 worldMatrix;

	Transform::Transform()
	{
		XMStoreFloat4x4(&trans, XMMatrixIdentity());
		XMStoreFloat4x4(&rot, XMMatrixIdentity());
		XMStoreFloat4x4(&scale, XMMatrixIdentity());
		XMStoreFloat4x4(&worldMatrix, XMMatrixIdentity());
	}

	Transform::~Transform()
	{

	}

	void Transform::Rotate(XMFLOAT3 rotation)
	{
		XMStoreFloat4x4(&this->rot,  XMMatrixMultiply(XMLoadFloat4x4(&this->rot),XMMatrixRotationZ(rotation.z) * XMMatrixRotationY(rotation.y) *  XMMatrixRotationX(rotation.x)));
	 }

	void Transform::Translate(XMFLOAT3 translation)
	{
		XMStoreFloat4x4(&this->trans, XMMatrixMultiply(XMLoadFloat4x4(&this->trans), XMMatrixTranslation(translation.x, translation.y, translation.z)));
	}

	void Transform::Scale(XMFLOAT3 scale)
	{
		XMStoreFloat4x4(&this->scale, XMMatrixMultiply(XMLoadFloat4x4(&this->scale), XMMatrixScaling(scale.x, scale.y, scale.z)));
	}
};
#endif