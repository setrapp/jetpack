#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

#include <DirectXMath.h>
#include <d3d11.h>
#include <vector>

using namespace DirectX;
using namespace std;

class Transform
{
public :
	DirectX::XMFLOAT4X4 trans, rot, scale;
	DirectX::XMFLOAT3 right, up, forward;
	DirectX::XMFLOAT4X4 worldMatrix;
	Transform* parent;
	vector<Transform*>children;

	Transform::Transform()
	{
		XMStoreFloat4x4(&trans, XMMatrixIdentity());
		XMStoreFloat4x4(&rot, XMMatrixIdentity());
		XMStoreFloat4x4(&scale, XMMatrixIdentity());
		XMStoreFloat4x4(&worldMatrix, XMMatrixIdentity());
		right = XMFLOAT3(1, 0, 0);
		up = XMFLOAT3(0, 1, 0);
		forward = XMFLOAT3(0, 0, 1);
	}

	Transform::~Transform()
	{

	}

	inline void Transform::Rotate(XMFLOAT3 rotation)
	{
		XMStoreFloat4x4(&this->rot,  XMMatrixMultiply(XMLoadFloat4x4(&this->rot),XMMatrixRotationZ(rotation.z) * XMMatrixRotationY(rotation.y) *  XMMatrixRotationX(rotation.x)));
		XMStoreFloat3(&this->right, XMVector3Transform(XMLoadFloat3(&this->right), XMLoadFloat4x4(&this->rot)));
		XMStoreFloat3(&this->up, XMVector3Transform(XMLoadFloat3(&this->up), XMLoadFloat4x4(&this->rot)));
		XMStoreFloat3(&this->forward, XMVector3Transform(XMLoadFloat3(&this->forward), XMLoadFloat4x4(&this->rot)));
		// TODO does child need to get parent transform when added???
		/*for (vector<Transform*>::iterator it = children.begin(); it < children.end(); it++)
		{
			(*it)->Rotate(rotation);
		}*/
	 }

	inline void Transform::Translate(XMFLOAT3 translation)
	{
		XMStoreFloat4x4(&this->trans, XMMatrixMultiply(XMLoadFloat4x4(&this->trans), XMMatrixTranslation(translation.x, translation.y, translation.z)));
	}

	inline void Transform::Scale(XMFLOAT3 scale)
	{
		XMStoreFloat4x4(&this->scale, XMMatrixMultiply(XMLoadFloat4x4(&this->scale), XMMatrixScaling(scale.x, scale.y, scale.z)));
	}

	void Transform::SetParent(Transform* parent)
	{
		/*if (parent == this->parent)
		{
			return;
		}
		
		// Remove from old parent.
		if (this->parent)
		{
			for(vector<Transform*>::iterator it = parent->children.begin(); it < parent->children.end(); it++)
			{
				if (*it == this)
				{
					parent->children.erase(it);
					it--;
				}
			}
		}

		// Add to new parent.
		this->parent = parent;
		if (this->parent) 
		{
			this->parent->children.push_back(this);
		}*/
	}

	/*XMFLOAT3 GetTranslation()
	{
		return 
	}*/
};
#endif