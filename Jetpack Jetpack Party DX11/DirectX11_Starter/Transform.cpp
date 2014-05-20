#include "Transform.h"

#pragma once

#include <DirectXMath.h>
#include <d3d11.h>
#include <vector>
#include <math.h>
#include "Entity.h"

using namespace DirectX;
using namespace std;

///////////////////////////////////////////////////
// NOTE: World and Local Matrices are Transposed //
///////////////////////////////////////////////////

Transform::Transform()
{
	parent = NULL;
	entity = NULL;
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
	XMStoreFloat3x3(&rotationMatrix, XMMatrixMultiply(XMLoadFloat3x3(&rotationMatrix), XMMatrixRotationAxis(XMLoadFloat3(&forward), rotation.z)));
	XMStoreFloat3x3(&rotationMatrix, XMMatrixMultiply(XMLoadFloat3x3(&rotationMatrix), XMMatrixRotationAxis(XMLoadFloat3(&up),rotation.y)));
	XMStoreFloat3x3(&rotationMatrix, XMMatrixMultiply(XMLoadFloat3x3(&rotationMatrix), XMMatrixRotationAxis(XMLoadFloat3(&right),rotation.x)));

	XMStoreFloat3x3(&this->rotation, XMMatrixMultiply(XMLoadFloat3x3(&this->rotation), XMLoadFloat3x3(&rotationMatrix)));
	UpdateLocalAndWorld();
		
	right = TransformDirection(XMFLOAT3(1, 0, 0));
	up = TransformDirection(XMFLOAT3(0, 1, 0));
	forward = TransformDirection(XMFLOAT3(0, 0, 1));
}

// Rotate locally.
void Transform::Rotate(const XMFLOAT3X3 rotation)
{
	Rotate(RotationToEuler(&rotation));
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
		eye = parent->InverseTransformPoint(eye);
		lookAt = parent->InverseTransformPoint(lookAt);
		up = parent->InverseTransformDirection(up);
	}
	
	XMFLOAT4X4 lookAtMatrix;
	XMStoreFloat4x4(&lookAtMatrix, XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&lookAt), XMLoadFloat3(&up)));
	XMStoreFloat4x4(&localMatrix, XMMatrixInverse(nullptr, XMLoadFloat4x4(&lookAtMatrix)));
	XMFLOAT3 newTranslation = XMFLOAT3(localMatrix._41, localMatrix._42, localMatrix._43);
	XMFLOAT3X3 newRotation;
	XMStoreFloat3x3(&newRotation, XMLoadFloat4x4(&localMatrix));
	SetLocalTranslation(newTranslation);
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
	XMMATRIX oldParentWorldMatrix = XMMatrixIdentity();
	if (this->parent)
	{
		oldParentWorldMatrix = XMLoadFloat4x4(&this->parent->GetWorldMatrix());
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
	XMStoreFloat4x4(&localMatrix, oldParentWorldMatrix * XMMatrixInverse(nullptr, parentWorldMatrix) * XMLoadFloat4x4(&localMatrix));

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

	if (entity && parent && parent->entity && !parent->entity->GetVisible())
	{
		entity->SetVisible(false);
	}
}

// Transform point from local space to world space.
XMFLOAT3 Transform::TransformPoint(XMFLOAT3 localPoint) const
{
	XMStoreFloat3(&localPoint, XMVector3Transform(XMLoadFloat3(&localPoint), XMMatrixTranspose(XMLoadFloat4x4(&worldMatrix))));
	return localPoint;
}

// Transform point from world space to local space.
XMFLOAT3 Transform::InverseTransformPoint(XMFLOAT3 worldPoint) const
{
	XMStoreFloat3(&worldPoint, XMVector3Transform(XMLoadFloat3(&worldPoint), XMMatrixInverse(nullptr, XMMatrixTranspose(XMLoadFloat4x4(&worldMatrix)))));
	return worldPoint;
}

// Transform direction from local space to world space.
XMFLOAT3 Transform::TransformDirection(XMFLOAT3 localDirection) const
{
	XMFLOAT3X3 sansTranslationAndScale;
	XMStoreFloat3x3(&sansTranslationAndScale, XMLoadFloat4x4(&worldMatrix));
	XMFLOAT3 worldScale = GetScale();
	sansTranslationAndScale._11 /= worldScale.x;
	sansTranslationAndScale._22 /= worldScale.y;
	sansTranslationAndScale._33 /= worldScale.z;
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

XMFLOAT3 Transform::ClampVector(XMFLOAT3* vector, float maxMagnitude, float minMagnitude) const
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

XMFLOAT3 Transform::RotationToEuler(XMFLOAT3X3 const* rotationMatrix, XMFLOAT3 expectedForward) const
{
	XMFLOAT3 eulerAngles(0, 0, 0);
	if (!rotationMatrix)
	{
		return eulerAngles;
	}

	// Compute the Euler Angles based on the value rotation around the y-axis.
	// If rotated exactly (+/-)PI/2 around the y-axis, pick a solution from the infinite possiblities.
	if (rotationMatrix->_31 == 1)
	{
		eulerAngles.y = PI / 2;
		eulerAngles.x = eulerAngles.z + atan2(rotationMatrix->_12, rotationMatrix->_13);
	}
	else if (rotationMatrix->_31 == -1)
	{
		eulerAngles.y = -PI / 2;
		eulerAngles.x = -eulerAngles.z + atan2(-rotationMatrix->_12, -rotationMatrix->_13);
	}
	else
	{
		eulerAngles.y = asin(rotationMatrix->_31);

		// Determine which of the possible angles to use testing against the rotations matrix's effects.
		XMFLOAT3 expectedForward = XMFLOAT3(0, 0, 1), testForward1 = XMFLOAT3(0, 0, 1), testForward2 = XMFLOAT3(0, 0, 1);
		float test1DotForward, test2DotForward;
		XMStoreFloat3(&expectedForward, XMVector3Transform(XMLoadFloat3(&expectedForward), XMLoadFloat3x3(rotationMatrix)));
		XMStoreFloat3(&testForward1, XMVector3Transform(XMLoadFloat3(&testForward1), XMMatrixRotationY(eulerAngles.y)));
		XMStoreFloat3(&testForward2, XMVector3Transform(XMLoadFloat3(&testForward2), XMMatrixRotationY(PI - eulerAngles.y)));
		XMStoreFloat(&test1DotForward, XMVector3Dot(XMLoadFloat3(&expectedForward), XMLoadFloat3(&testForward1)));
		XMStoreFloat(&test2DotForward, XMVector3Dot(XMLoadFloat3(&expectedForward), XMLoadFloat3(&testForward2)));
		if (test2DotForward > test1DotForward)
		{
			eulerAngles.y = PI - eulerAngles.y;
		}

		float cosY = cos(eulerAngles.y);
		eulerAngles.x = -atan2(rotationMatrix->_32 / cosY, rotationMatrix->_33 / cosY);
		eulerAngles.z = -atan2(rotationMatrix->_21 / cosY, rotationMatrix->_11 / cosY);
	}
	
	return eulerAngles;
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
	Translate(InverseTransformDirection(translation));
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

XMFLOAT3 Transform::GetEulerAngles() const
{
	return RotationToEuler(&GetRotation(), forward);
}

XMFLOAT3X3 Transform::GetLocalRotation() const
{
	return rotation;
}

XMFLOAT3 Transform::GetLocalEulerAngles() const
{
	XMFLOAT3 expectedForward;
	XMStoreFloat3(&expectedForward, XMVector3Transform(XMLoadFloat3(&XMFLOAT3(0, 0, 1)), XMLoadFloat3x3(&rotation)));
	return RotationToEuler(&rotation, expectedForward);
}

void Transform::SetLocalRotation(XMFLOAT3 newEulerAngles)
{
	ApplyRotation(newEulerAngles);
}

void Transform::SetLocalRotation(const XMFLOAT3 axis, const float newAngle)
{
	XMFLOAT3X3 rotation;
	XMStoreFloat3x3(&rotation, XMMatrixRotationAxis(XMLoadFloat3(&axis), newAngle));
	SetLocalRotation(rotation);
}

void Transform::SetLocalRotation(const XMFLOAT3X3 newRotation)
{
	ApplyRotation(RotationToEuler(&newRotation));
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
		XMStoreFloat3(&up, XMVector3Normalize(upVector));
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

void Transform::ApplyRotation(XMFLOAT3 rotation)
{
	XMFLOAT3 eulerAngles;
	XMStoreFloat3x3(&this->rotation, XMMatrixIdentity());
	if (parent)
	{
		right = parent->TransformDirection(XMFLOAT3(1, 0, 0));
		up = parent->TransformDirection(XMFLOAT3(0, 1, 0));
		forward = parent->TransformDirection(XMFLOAT3(0, 0, 1));
	}
	else
	{
		right = XMFLOAT3(1, 0, 0);
		up = XMFLOAT3(0, 1, 0);
		forward = XMFLOAT3(0, 0, 1);
	}
	Rotate(rotation);
	UpdateLocalAndWorld();
}