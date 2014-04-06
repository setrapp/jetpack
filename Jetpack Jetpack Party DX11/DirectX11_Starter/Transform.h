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
	DirectX::XMFLOAT4X4 localMatrix; // should be private
	Transform* parent; // should be private
	vector<Transform*>children; // should be private

private:
	DirectX::XMFLOAT3 trans, rot, scale;
	DirectX::XMFLOAT3 right, up, forward;
	
	

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
		XMStoreFloat4x4(&localMatrix, XMMatrixIdentity());
		XMStoreFloat4x4(&worldMatrix, XMMatrixIdentity());
	}

	Transform::~Transform()
	{

	}

	// Rotate locally.
	void Transform::Rotate(XMFLOAT3 rotation)
	{
		XMMATRIX rotationMatrix = XMMatrixRotationX(rotation.x) * XMMatrixRotationY(rotation.y) * XMMatrixRotationZ(rotation.z);
		XMStoreFloat4x4(&localMatrix,  XMMatrixMultiply(XMLoadFloat4x4(&localMatrix), XMMatrixTranspose(rotationMatrix)));
		UpdateWorldMatrix();
		
		XMStoreFloat3(&right, XMVector3Transform(XMLoadFloat3(&right), rotationMatrix));
		XMStoreFloat3(&up, XMVector3Transform(XMLoadFloat3(&up), rotationMatrix));
		XMStoreFloat3(&forward, XMVector3Transform(XMLoadFloat3(&forward), rotationMatrix));
		// TODO does child need to get parent transform when added???
		/*for (vector<Transform*>::iterator it = children.begin(); it < children.end(); it++)
		{
			(*it)->Rotate(rotation);
		}*/
	 }

	// Translate locally.
	void Transform::Translate(XMFLOAT3 translation)
	{
		XMStoreFloat4x4(&localMatrix, XMMatrixMultiply(XMLoadFloat4x4(&localMatrix), XMMatrixTranspose(XMMatrixTranslation(translation.x, translation.y, translation.z))));
		UpdateWorldMatrix();
		for (vector<Transform*>::iterator it = children.begin(); it < children.end(); it++) {
			(*it)->Translate(translation);
		}
	}

	// Scale locally.
	void Transform::Scale(XMFLOAT3 scale)
	{
		XMStoreFloat4x4(&localMatrix, XMMatrixMultiply(XMLoadFloat4x4(&localMatrix), XMMatrixScaling(scale.x, scale.y, scale.z)));
		UpdateWorldMatrix();
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

	// Get world translation.
	XMFLOAT3 GetTranslation()
	{
		return XMFLOAT3(worldMatrix._41, worldMatrix._42, worldMatrix._43);
	}

	// Get local translation.
	XMFLOAT3 GetLocalTranslation()
	{
		return XMFLOAT3(localMatrix._41, localMatrix._42, localMatrix._43);
	}

	// Set world translation
	void SetTranslation(XMFLOAT3 newPosition)
	{
		if (parent) 
		{
			XMStoreFloat3(&newPosition, XMVector3Transform(XMLoadFloat3(&newPosition), XMLoadFloat4x4(&parent->worldMatrix)));
		}
		SetLocalTranslation(newPosition);
	}

	// Set local translation
	void SetLocalTranslation(XMFLOAT3 newPosition)
	{
		XMFLOAT3 translation;
		XMStoreFloat3(&translation, XMVectorSubtract(XMLoadFloat3(&newPosition), XMLoadFloat3(&XMFLOAT3(localMatrix._41, localMatrix._42, localMatrix._43))));
		Translate(translation);
	}

	XMFLOAT3X3 GetRotation()
	{
		return XMFLOAT3X3(	1, worldMatrix._12, worldMatrix._13,
							worldMatrix._21, 1, worldMatrix._23,
							worldMatrix._31, worldMatrix._32, 1);
	}

	XMFLOAT3X3 GetLocalRotation()
	{
		return XMFLOAT3X3(	1, localMatrix._12, localMatrix._13,
							localMatrix._21, 1, localMatrix._23,
							localMatrix._31, localMatrix._32, 1);
	}

	void SetRotation(XMFLOAT3 newEulerAngles)
	{
		if (parent)
		{
			XMStoreFloat3(&newEulerAngles, XMVector3Transform(XMLoadFloat3(&newEulerAngles), XMLoadFloat4x4(&parent->worldMatrix)));
		}
		SetLocalRotation(newEulerAngles);
	}

	void SetRotation(XMFLOAT3 axis, float newAngle)
	{
		if (parent)
		{
			XMStoreFloat3(&axis, XMVector3Transform(XMLoadFloat3(&axis), XMLoadFloat4x4(&parent->worldMatrix)));
		}
		SetLocalRotation(axis, newAngle);
	}

	void SetRotation(XMFLOAT3X3 newRotation)
	{
		if (parent)
		{
			XMStoreFloat3x3(&newRotation, XMMatrixMultiply(XMLoadFloat3x3(&newRotation), XMLoadFloat4x4(&parent->worldMatrix)));
		}
		SetLocalRotation(newRotation);
	}

	void SetLocalRotation(XMFLOAT3 newEulerAngles)
	{
		XMMATRIX rotation = XMMatrixRotationRollPitchYaw(newEulerAngles.x, newEulerAngles.y, newEulerAngles.z);
		ApplyRotation(&rotation);
	}

	void SetLocalRotation(XMFLOAT3 axis, float newAngle)
	{
		XMMATRIX rotation = XMMatrixRotationAxis(XMLoadFloat3(&axis), newAngle);
		ApplyRotation(&rotation);
	}

	void SetLocalRotation(XMFLOAT3X3 newRotation)
	{
		ApplyRotation(&XMLoadFloat3x3(&newRotation));
	}

	XMFLOAT3 GetScale()
	{
		return XMFLOAT3(worldMatrix._11, worldMatrix._22, worldMatrix._33);
	}

	XMFLOAT3 GetLocalScale()
	{
		return XMFLOAT3(localMatrix._11, localMatrix._22, localMatrix._33);
	}

	// Set world scale
	void SetScale(XMFLOAT3 newScale)
	{
		if (parent) 
		{
			XMStoreFloat3(&newScale, XMVector3Transform(XMLoadFloat3(&newScale), XMLoadFloat4x4(&parent->worldMatrix)));
		}
		SetLocalTranslation(newScale);
	}

	// Set local scale
	void SetLocalScale(XMFLOAT3 newScale)
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
		if (parent) 
		{
			XMStoreFloat4x4(&worldMatrix, XMMatrixMultiply(XMLoadFloat4x4(&localMatrix), XMLoadFloat4x4(&parent->worldMatrix)));
		} else {
			XMStoreFloat4x4(&worldMatrix, XMLoadFloat4x4(&localMatrix));
		}
	}

	void ApplyRotation(XMMATRIX* rotation)
	{
		localMatrix._12 = localMatrix._13 = localMatrix._21 = localMatrix._23 = localMatrix._31 = localMatrix._32 = 0;
		XMStoreFloat4x4(&localMatrix, XMMatrixMultiply(XMLoadFloat4x4(&localMatrix), *rotation));
		UpdateWorldMatrix();
	}

	void UpdateChildOrbit()
	{
		//TODO Set child rotations around parent to Identity
	}
};
#endif