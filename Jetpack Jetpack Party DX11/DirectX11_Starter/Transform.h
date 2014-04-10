#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

#include <DirectXMath.h>
#include <d3d11.h>
#include <vector>

using namespace DirectX;
using namespace std;

///////////////////////////////////////////////////
// NOTE: World and Local Matrices are Transposed //
///////////////////////////////////////////////////

class Transform
{
public:
	DirectX::XMFLOAT4X4 worldMatrix; // should be private
	DirectX::XMFLOAT4X4 localMatrix; // should be private
	Transform* parent; // should be private
	vector<Transform*>children; // should be private

private:
	DirectX::XMFLOAT3 right, up, forward;
	
	

public:

	Transform::Transform()
	{
		right = XMFLOAT3(1, 0, 0);
		up = XMFLOAT3(0, 1, 0);
		forward = XMFLOAT3(0, 0, 1);
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
	 }

	// Translate locally.
	void Transform::Translate(XMFLOAT3 translation)
	{
		XMStoreFloat4x4(&localMatrix, XMMatrixMultiply(XMLoadFloat4x4(&localMatrix), XMMatrixTranspose(XMMatrixTranslation(translation.x, translation.y, translation.z))));
		UpdateWorldMatrix();
	}

	// Scale locally.
	void Transform::Scale(XMFLOAT3 scale)
	{
		XMStoreFloat4x4(&localMatrix, XMMatrixMultiply(XMLoadFloat4x4(&localMatrix), XMMatrixScaling(scale.x, scale.y, scale.z)));
		UpdateWorldMatrix();
	}

	void Transform::SetParent(Transform* parent)
	{
		if (parent == this->parent)
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
		XMMATRIX parentWorldMatrix = XMMatrixIdentity();
		if (this->parent) 
		{
			this->parent->children.push_back(this);
			parentWorldMatrix = XMLoadFloat4x4(&parent->worldMatrix);
		}

		// Update local matrix to exist within new parent space.
		XMStoreFloat4x4(&localMatrix, XMMatrixMultiply(XMLoadFloat4x4(&worldMatrix), XMMatrixInverse(nullptr, parentWorldMatrix)));
	}

	// Transform point from local space to world space.
	XMFLOAT3 TransformPoint(XMFLOAT3 localPoint)
	{
		if (parent)
		{
			XMStoreFloat3(&localPoint, XMVector3Transform(XMLoadFloat3(&localPoint), XMLoadFloat4x4(&parent->worldMatrix)));
		}
		return localPoint;
	}

	// Transform point from world space to local space.
	XMFLOAT3 InverseTransformPoint(XMFLOAT3 worldPoint)
	{
		if (parent)
		{
			XMStoreFloat3(&worldPoint, XMVector3Transform(XMLoadFloat3(&worldPoint), XMMatrixInverse(nullptr, XMLoadFloat4x4(&parent->worldMatrix))));
		}
		return worldPoint;
	}

	// Transform direction from local space to world space.
	XMFLOAT3 TransformDirection(XMFLOAT3 localDirection)
	{
		if (parent)
		{
			XMFLOAT3X3 sansTranslation;
			XMStoreFloat3x3(&sansTranslation, XMLoadFloat4x4(&parent->worldMatrix));
			XMStoreFloat3(&localDirection, XMVector3Transform(XMLoadFloat3(&localDirection), XMLoadFloat3x3(&sansTranslation)));
		}
		return localDirection;
	}

	// Transform direction from world space to local space.
	XMFLOAT3 InverseTransformDirection(XMFLOAT3 worldDirection)
	{
		if (parent)
		{
			XMFLOAT3X3 sansTranslation;
			XMStoreFloat3x3(&sansTranslation, XMLoadFloat4x4(&parent->worldMatrix));
			XMStoreFloat3(&worldDirection, XMVector3Transform(XMLoadFloat3(&worldDirection), XMMatrixInverse(nullptr, XMLoadFloat3x3(&sansTranslation))));
		}
		return worldDirection;
	}

	// Get world translation.
	XMFLOAT3 GetTranslation()
	{
		return XMFLOAT3(worldMatrix._14, worldMatrix._24, worldMatrix._34);
	}

	// Get local translation.
	XMFLOAT3 GetLocalTranslation()
	{
		return XMFLOAT3(localMatrix._14, localMatrix._24, localMatrix._34);
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
		XMStoreFloat3(&translation, XMVectorSubtract(XMLoadFloat3(&newPosition), XMLoadFloat3(&GetLocalTranslation())));
		Translate(translation);
	}

	XMFLOAT3X3 GetRotation()
	{
		return XMFLOAT3X3(	1, worldMatrix._21, worldMatrix._31,
							worldMatrix._12, 1, worldMatrix._32,
							worldMatrix._13, worldMatrix._23, 1);
	}

	XMFLOAT3X3 GetLocalRotation()
	{
		return XMFLOAT3X3(	1, localMatrix._21, localMatrix._31,
							localMatrix._12, 1, localMatrix._32,
							localMatrix._13, localMatrix._23, 1);
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
		SetLocalScale(newScale);
	}

	// Set local scale
	void SetLocalScale(XMFLOAT3 newScale)
	{
		XMFLOAT3 scale;
		XMStoreFloat3(&scale, XMVectorDivide(XMLoadFloat3(&newScale), XMLoadFloat3(&GetLocalScale())));
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

	void UpdateWorldMatrix()
	{
		if (parent) 
		{
			XMStoreFloat4x4(&worldMatrix, XMMatrixMultiply(XMLoadFloat4x4(&parent->worldMatrix), XMLoadFloat4x4(&localMatrix)));
		} else {
			XMStoreFloat4x4(&worldMatrix, XMLoadFloat4x4(&localMatrix));
		}

		for (vector<Transform*>::iterator it = children.begin(); it < children.end(); it++) {
			(*it)->UpdateWorldMatrix();
		}
	}

private:
	void ApplyRotation(XMMATRIX* rotation)
	{
		//XMFLOAT3 oldScale(localMatrix._11, localMatrix._22, localMatrix._33);
		XMFLOAT3X3 rotationMatrix = GetLocalRotation();
		//XMStoreFloat3x3(&rotationMatrix, XMMatrixTranspose(XMLoadFloat3x3(&rotationMatrix)));
		XMStoreFloat4x4(&localMatrix, XMMatrixMultiply(XMLoadFloat4x4(&localMatrix), XMLoadFloat3x3(&rotationMatrix)));
		//XMStoreFloat3x3(&rotationMatrix, XMMatrixMultiply(XMLoadFloat3x3(&rotationMatrix), *rotation));
		//localMatrix._12 = localMatrix._13 = localMatrix._21 = localMatrix._23 = localMatrix._31 = localMatrix._32 = 0;
		XMStoreFloat4x4(&localMatrix, XMMatrixMultiply(XMLoadFloat4x4(&localMatrix), XMMatrixTranspose(*rotation)));
		UpdateWorldMatrix();
	}
};
#endif