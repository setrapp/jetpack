#include "Transform.h"

#pragma once

#include <DirectXMath.h>
#include <d3d11.h>
#include <vector>
#include <math.h>

using namespace DirectX;
using namespace std;

///////////////////////////////////////////////////
// NOTE: World and Local Matrices are Transposed //
///////////////////////////////////////////////////

Transform::Transform()
{
	parent = NULL;
	right = XMFLOAT3(1, 0, 0);
	up = XMFLOAT3(0, 1, 0);
	forward = XMFLOAT3(0, 0, 1);
	XMStoreFloat4x4(&localMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&worldMatrix, XMMatrixIdentity());
	translation = XMFLOAT3(0, 0, 0);
	XMStoreFloat3x3(&rotation, XMMatrixIdentity());
	scale = XMFLOAT3(1, 1, 1);
}

Transform::~Transform()
{
	
}

// Translate locally.
void Transform::Translate(const XMFLOAT3 translation)
{
	XMFLOAT3 actualTranslation = TransformDirection(translation);
	XMStoreFloat3(&this->translation, XMVector3Transform(XMLoadFloat3(&this->translation), XMMatrixTranslation(actualTranslation.x, actualTranslation.y, actualTranslation.z)));
	UpdateLocalAndWorld();
}

// Rotate locally.
void Transform::Rotate(const XMFLOAT3 rotation)
{
	XMFLOAT3X3 rotationMatrix;
	XMStoreFloat3x3(&rotationMatrix, XMMatrixIdentity());
	XMStoreFloat3x3(&rotationMatrix, XMMatrixMultiply(XMLoadFloat3x3(&rotationMatrix), XMMatrixRotationZ(rotation.z)));
	XMStoreFloat3x3(&rotationMatrix, XMMatrixMultiply(XMLoadFloat3x3(&rotationMatrix), XMMatrixRotationY(rotation.y)));
	XMStoreFloat3x3(&rotationMatrix, XMMatrixMultiply(XMLoadFloat3x3(&rotationMatrix), XMMatrixRotationX(rotation.x)));
	Rotate(rotationMatrix);
}

// Rotate locally.
void Transform::Rotate(const XMFLOAT3X3 rotation)
{
	XMStoreFloat3x3(&this->rotation, XMMatrixMultiply(XMLoadFloat3x3(&this->rotation), XMLoadFloat3x3(&rotation)));
	UpdateLocalAndWorld();
		
	right = TransformDirection(XMFLOAT3(1, 0, 0));
	up = TransformDirection(XMFLOAT3(0, 1, 0));
	forward = TransformDirection(XMFLOAT3(0, 0, 1));
}

// Scale locally.
void Transform::Scale(const XMFLOAT3 scale)
{
	XMStoreFloat3(&this->scale, XMVector3Transform(XMLoadFloat3(&this->scale), XMMatrixScaling(scale.x, scale.y, scale.z)));
	UpdateLocalAndWorld();
}

void Transform::LookAt(XMFLOAT3 eye, XMFLOAT3 lookAt, XMFLOAT3 up)
{
	XMVECTOR newForward = XMVector3Normalize(XMVectorSubtract(XMLoadFloat3(&lookAt), XMLoadFloat3(&eye)));
	XMVECTOR newUp = XMVector3Normalize(XMLoadFloat3(&up));
	XMVECTOR newForwardDotUp = XMVector3Dot(newForward, newUp);
	XMFLOAT3 product;
	XMStoreFloat3(&product, XMVector3Dot(newForward, newUp));
	if (product.x > 0.99f)
	{
		newUp = XMVector3Normalize(XMVector3Cross(newForward, XMLoadFloat3(&right)));
	}
	XMStoreFloat3(&up, newUp);

	if (parent)
	{
		eye = parent->TransformPoint(eye);
		lookAt = parent->TransformPoint(lookAt);
		up = parent->TransformDirection(up);
	}
	
	XMFLOAT4X4 lookAtMatrix;
	XMStoreFloat4x4(&lookAtMatrix, XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&lookAt), XMLoadFloat3(&up)));
	XMStoreFloat4x4(&localMatrix, XMMatrixInverse(nullptr, XMLoadFloat4x4(&lookAtMatrix)));
	SetTranslation(XMFLOAT3(localMatrix._41, localMatrix._42, localMatrix._43));
	XMFLOAT3X3 newRotation;
	XMStoreFloat3x3(&newRotation, XMMatrixTranspose(XMLoadFloat4x4(&localMatrix)));
	SetLocalRotation(newRotation);
	UpdateLocalAndWorld();
}

XMFLOAT4X4 Transform::GetWorldMatrix()  const
{
	return worldMatrix;
}

XMFLOAT4X4 Transform::GetLocalMatrix() const
{
	return localMatrix;
}


Transform* Transform::GetParent() const
{
	return parent;
}

void Transform::SetParent(Transform* parent)
{
	if (parent == this->parent || this->parent == this)
	{
		return;
	}
		
	// Remove from old parent.
	if (this->parent)
	{
		vector<Transform*>::iterator it = this->parent->children.begin();
		while(it != this->parent->children.end())
		{
			if (*it == this)
			{
				it = this->parent->children.erase(it);
			}
			else
			{
				it++;
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

	// Update translation, rotation, and scale
	translation = XMFLOAT3(localMatrix._14, localMatrix._24, localMatrix._34);
	scale = XMFLOAT3(	sqrt((localMatrix._11 * localMatrix._11) + (localMatrix._12 * localMatrix._12) + (localMatrix._13 * localMatrix._13)), 
						sqrt((localMatrix._21 * localMatrix._21) + (localMatrix._22 * localMatrix._22) + (localMatrix._23 * localMatrix._23)),
						sqrt((localMatrix._31 * localMatrix._31) + (localMatrix._32 * localMatrix._32) + (localMatrix._33 * localMatrix._33)));
	XMStoreFloat3x3(&rotation, XMLoadFloat4x4(&localMatrix));
	rotation._11 /= scale.x;
	rotation._22 /= scale.y;
	rotation._33 /= scale.z;
	UpdateLocalAndWorld();
}

// Transform point from local space to world space.
XMFLOAT3 Transform::TransformPoint(XMFLOAT3 localPoint) const
{
	XMStoreFloat3(&localPoint, XMVector3Transform(XMLoadFloat3(&localPoint), XMMatrixTranspose(XMMatrixInverse(nullptr, XMLoadFloat4x4(&worldMatrix)))));
	return localPoint;
}

// Transform point from world space to local space.
XMFLOAT3 Transform::InverseTransformPoint(XMFLOAT3 worldPoint) const
{
	XMStoreFloat3(&worldPoint, XMVector3Transform(XMLoadFloat3(&worldPoint), XMLoadFloat4x4(&worldMatrix)));
	return worldPoint;
}

// Transform direction from local space to world space.
XMFLOAT3 Transform::TransformDirection(XMFLOAT3 localDirection) const
{
	XMFLOAT3X3 sansTranslationAndScale;
	XMStoreFloat3x3(&sansTranslationAndScale, XMLoadFloat4x4(&worldMatrix));
	sansTranslationAndScale._11 /= scale.x;
	sansTranslationAndScale._22 /= scale.y;
	sansTranslationAndScale._33 /= scale.z;
	XMStoreFloat3(&localDirection, XMVector3Transform(XMLoadFloat3(&localDirection), XMMatrixTranspose(XMLoadFloat3x3(&sansTranslationAndScale))));
	return localDirection;
}

// Transform direction from world space to local space.
XMFLOAT3 Transform::InverseTransformDirection(XMFLOAT3 worldDirection) const
{
	XMFLOAT3X3 sansTranslationAndScale;
	XMStoreFloat3x3(&sansTranslationAndScale, XMLoadFloat4x4(&worldMatrix));
	sansTranslationAndScale._11 /= scale.x;
	sansTranslationAndScale._22 /= scale.y;
	sansTranslationAndScale._33 /= scale.z;
	XMStoreFloat3(&worldDirection, XMVector3Transform(XMLoadFloat3(&worldDirection), XMLoadFloat3x3(&sansTranslationAndScale)));
	return worldDirection;
}

XMFLOAT3 Transform::ClampVector(XMFLOAT3* vector, float maxMagnitude, float minMagnitude)
{
	if (minMagnitude > maxMagnitude)
	{
		return *vector;
	}
	XMVECTOR vec = XMLoadFloat3(vector);
	XMFLOAT3 vecMag3;
	XMStoreFloat3(&vecMag3, XMVector3Length(vec));
	float velocityMag = vecMag3.x;
	if (velocityMag > maxMagnitude) {
		vector->x *= (maxMagnitude / velocityMag);
		vector->y *= (maxMagnitude / velocityMag);
		vector->z *= (maxMagnitude / velocityMag);
	} else if (velocityMag < minMagnitude) {
		vector->x *= (minMagnitude / velocityMag);
		vector->y *= (minMagnitude / velocityMag);
		vector->z *= (minMagnitude / velocityMag);
	}
}

// Get world translation.
XMFLOAT3 Transform::GetTranslation() const
{
	return XMFLOAT3(worldMatrix._14, worldMatrix._24, worldMatrix._34);
}

// Get local translation.
XMFLOAT3 Transform::GetLocalTranslation() const
{
	return translation;
}

// Set world translation
void Transform::SetTranslation(XMFLOAT3 newPosition)
{
	if (parent) 
	{
		XMStoreFloat3(&newPosition, XMVector3Transform(XMLoadFloat3(&newPosition), XMLoadFloat4x4(&parent->worldMatrix)));
	}
	SetLocalTranslation(newPosition);
}

// Set local translation
void Transform::SetLocalTranslation(XMFLOAT3 newPosition)
{
	XMFLOAT3 translation;
	XMStoreFloat3(&translation, XMVectorSubtract(XMLoadFloat3(&newPosition), XMLoadFloat3(&GetLocalTranslation())));
	Translate(translation);
}

XMFLOAT3X3 Transform::GetRotation() const
{
	XMFLOAT3X3 worldRotation;
	if (parent)
	{
		XMStoreFloat3x3(&worldRotation, XMMatrixMultiply(XMLoadFloat3x3(&parent->GetRotation()), XMLoadFloat3x3(&rotation))); 
	}
	else
	{
		worldRotation = rotation; 
	}
	return worldRotation;
}

XMFLOAT3X3 Transform::GetLocalRotation() const
{
	return rotation;
}

void Transform::SetLocalRotation(XMFLOAT3 newEulerAngles)
{
	XMMATRIX rotation = XMMatrixRotationRollPitchYaw(newEulerAngles.x, newEulerAngles.y, newEulerAngles.z);
	ApplyRotation(&rotation);
}

void Transform::SetLocalRotation(const XMFLOAT3 axis, const float newAngle)
{
	XMMATRIX rotation = XMMatrixRotationAxis(XMLoadFloat3(&axis), newAngle);
	ApplyRotation(&rotation);
}

void Transform::SetLocalRotation(const XMFLOAT3X3 newRotation)
{
	ApplyRotation(&XMLoadFloat3x3(&newRotation));
}

bool Transform::IsUniformScale() const
{
	float scaleEpsilon = 0.00001f;
	return ((abs(scale.x - scale.y) < scaleEpsilon) && (abs(scale.x - scale.z) < scaleEpsilon)) && (parent == NULL || parent->IsUniformScale());
}

XMFLOAT3 Transform::GetScale() const
{
	XMFLOAT3 worldScale;
	if (parent)
	{
		XMStoreFloat3(&worldScale, XMVectorMultiply(XMLoadFloat3(&parent->GetScale()), XMLoadFloat3(&scale)));
	}
	else
	{
		worldScale = scale;
	}
	return worldScale;
}

XMFLOAT3 Transform::GetLocalScale() const
{
	return scale;
}

// Set local scale
void Transform::SetLocalScale(XMFLOAT3 newScale)
{
	XMStoreFloat3(&newScale, XMVectorDivide(XMLoadFloat3(&newScale), XMLoadFloat3(&GetLocalScale())));
	Scale(newScale);
}

XMFLOAT3 Transform::GetForward()
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

XMFLOAT3 Transform::GetRight()
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

XMFLOAT3 Transform::GetUp()
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

// Concatenate translation, scale, and rotation in to local and world matrices.
void Transform::UpdateLocalAndWorld()
{
	XMStoreFloat4x4(&localMatrix, XMMatrixTranspose(XMMatrixScaling(scale.x, scale.y, scale.z) * XMLoadFloat3x3(&rotation) * XMMatrixTranslation(translation.x, translation.y, translation.z)));

	if (parent) 
	{
		XMStoreFloat4x4(&worldMatrix, XMMatrixMultiply(XMLoadFloat4x4(&parent->worldMatrix), XMLoadFloat4x4(&localMatrix)));
	} else {
		XMStoreFloat4x4(&worldMatrix, XMLoadFloat4x4(&localMatrix));
	}

	// Update child transforms.
	for (vector<Transform*>::iterator it = children.begin(); it < children.end(); it++) {
		(*it)->UpdateLocalAndWorld();
	}
}

void Transform::ApplyRotation(XMMATRIX* rotation)
{
	XMFLOAT3X3 rotationMatrix;
	XMStoreFloat3x3(&this->rotation, XMMatrixIdentity());
	XMStoreFloat3x3(&rotationMatrix, *rotation);
	Rotate(rotationMatrix);
	UpdateLocalAndWorld();
}