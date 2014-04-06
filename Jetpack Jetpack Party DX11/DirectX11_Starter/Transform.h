#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

#include <DirectXMath.h>
#include <d3d11.h>
#include <vector>

using namespace DirectX;
using namespace std;

class Transform
{
public:
	DirectX::XMFLOAT4X4 worldMatrix; // should be private

private:
	DirectX::XMFLOAT3 trans, rot, scale;
	DirectX::XMFLOAT3 right, up, forward;
	
	Transform* parent;
	vector<Transform*>children;

public:

	Transform::Transform()
	{
		/*XMStoreFloat4x4(&trans, XMMatrixIdentity());
		XMStoreFloat4x4(&rot, XMMatrixIdentity());
		XMStoreFloat4x4(&scale, XMMatrixIdentity());
		XMStoreFloat4x4(&worldMatrix, XMMatrixIdentity());*/
		/*right = XMFLOAT3(1, 0, 0);
		up = XMFLOAT3(0, 1, 0);
		forward = XMFLOAT3(0, 0, 1);*/
		XMStoreFloat4x4(&worldMatrix, XMMatrixIdentity());
	}

	Transform::~Transform()
	{

	}

	void Transform::Rotate(XMFLOAT3 rotation)
	{
		XMMATRIX rotationMatrix = XMMatrixRotationX(rotation.x) * XMMatrixRotationX(rotation.y) * XMMatrixRotationX(rotation.z);
		XMStoreFloat4x4(&worldMatrix,  XMMatrixMultiply(XMLoadFloat4x4(&worldMatrix), XMMatrixTranspose(rotationMatrix)));
		
		XMStoreFloat3(&right, XMVector3Transform(XMLoadFloat3(&this->right), rotationMatrix));
		XMStoreFloat3(&up, XMVector3Transform(XMLoadFloat3(&this->up), rotationMatrix));
		XMStoreFloat3(&forward, XMVector3Transform(XMLoadFloat3(&this->forward), rotationMatrix));
		// TODO does child need to get parent transform when added???
		/*for (vector<Transform*>::iterator it = children.begin(); it < children.end(); it++)
		{
			(*it)->Rotate(rotation);
		}*/
	 }

	void Transform::Translate(XMFLOAT3 translation)
	{
		XMStoreFloat4x4(&worldMatrix, XMMatrixMultiply(XMLoadFloat4x4(&worldMatrix), XMMatrixTranspose(XMMatrixTranslation(translation.x, translation.y, translation.z))));
	}

	void Transform::Scale(XMFLOAT3 scale)
	{
		XMStoreFloat4x4(&worldMatrix, XMMatrixMultiply(XMLoadFloat4x4(&worldMatrix), XMMatrixScaling(scale.x, scale.y, scale.z)));
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

	XMFLOAT3 GetTranslation()
	{
		return XMFLOAT3(worldMatrix._41, worldMatrix._42, worldMatrix._43);
	}

	void SetTranslation(XMFLOAT3 newPosition)
	{
		XMFLOAT3 translation;
		XMStoreFloat3(&translation, XMVectorSubtract(XMLoadFloat3(&newPosition), XMLoadFloat3(&XMFLOAT3(worldMatrix._41, worldMatrix._42, worldMatrix._43))));
		Translate(translation);
	}

	XMFLOAT3X3 GetRotation()
	{
		return XMFLOAT3X3(	1, worldMatrix._12, worldMatrix._13,
							worldMatrix._21, 1, worldMatrix._23,
							worldMatrix._31, worldMatrix._32, 1);
	}

	void SetRotation(XMFLOAT3 newEulerAngles)
	{
		XMMATRIX rotation = XMMatrixRotationRollPitchYaw(newEulerAngles.x, newEulerAngles.y, newEulerAngles.z);
		ApplyRotation(&rotation);
		
	}

	void SetRotation(XMFLOAT3 axis, float newAngle)
	{
		XMMATRIX rotation = XMMatrixRotationAxis(XMLoadFloat3(&axis), newAngle);
		ApplyRotation(&rotation);
	}

	void SetRotation(XMFLOAT3X3 newRotation)
	{
		ApplyRotation(&XMLoadFloat3x3(&newRotation));
	}

	XMFLOAT3 GetScale()
	{
		return XMFLOAT3(worldMatrix._11, worldMatrix._22, worldMatrix._33);
	}

	void SetScale(XMFLOAT3 newScale)
	{
		XMFLOAT3 scale;
		XMStoreFloat3(&scale, XMVectorDivide(XMLoadFloat3(&newScale), XMLoadFloat3(&XMFLOAT3(worldMatrix._11, worldMatrix._22, worldMatrix._33))));
		Scale(scale);
	}

	XMFLOAT3 GetForward()
	{
		XMVECTOR forwardVector = XMLoadFloat3(&forward);
		XMFLOAT3 lengthSq;
		XMStoreFloat3(&lengthSq, XMVector3LengthSq(forwardVector));
		if (lengthSq.x != 1.0f) 
		{
			XMStoreFloat3(&forward, XMVector3Normalize(forwardVector));
		}
		return forward;
	}

	XMFLOAT3 GetRight()
	{
		XMVECTOR rightVector = XMLoadFloat3(&right);
		XMFLOAT3 lengthSq;
		XMStoreFloat3(&lengthSq, XMVector3LengthSq(rightVector));
		if (lengthSq.x != 1.0f) 
		{
			XMStoreFloat3(&right, XMVector3Normalize(rightVector));
		}
		return right;
	}

	XMFLOAT3 GetUp()
	{
		XMVECTOR upVector = XMLoadFloat3(&up);
		XMFLOAT3 lengthSq;
		XMStoreFloat3(&lengthSq, XMVector3LengthSq(upVector));
		if (lengthSq.x != 1.0f) 
		{
			XMStoreFloat3(&forward, XMVector3Normalize(upVector));
		}
		return up;
	}

private:
	void UpdateWorldMatrix()
	{

	}

	void ApplyRotation(XMMATRIX* rotation)
	{
		worldMatrix._12 = worldMatrix._13 = worldMatrix._21 = worldMatrix._23 = worldMatrix._31 = worldMatrix._32 = 0;
		XMStoreFloat4x4(&worldMatrix, XMMatrixMultiply(XMLoadFloat4x4(&worldMatrix), *rotation));
	}

	void UpdateChildOrbit()
	{
		//TODO Set child rotations around parent to Identity
	}
};
#endif