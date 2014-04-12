#pragma once

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
private:
	XMFLOAT3 right, up, forward;
	XMFLOAT4X4 worldMatrix;
	XMFLOAT4X4 localMatrix;
	XMFLOAT3 translation;
	XMFLOAT3 scale;
	XMFLOAT3X3 rotation;
	Transform* parent;
	vector<Transform*>children;
	
public:

	Transform::Transform();

	Transform::~Transform();

	// Translate locally.
	void Transform::Translate(XMFLOAT3 translation);

	// Rotate locally.
	void Transform::Rotate(XMFLOAT3 rotation);

	// Rotate locally.
	void Transform::Rotate(XMFLOAT3X3 rotation);

	// Scale locally.
	void Transform::Scale(XMFLOAT3 scale);

	void Transform::LookAt(XMFLOAT3 eye, XMFLOAT3 lookAt, XMFLOAT3 up);

	XMFLOAT4X4 Transform::GetWorldMatrix();

	XMFLOAT4X4 Transform::GetLocalMatrix();

	Transform* Transform::GetParent();

	void Transform::SetParent(Transform* parent);

	// Transform point from local space to world space.
	XMFLOAT3 TransformPoint(XMFLOAT3 localPoint);

	// Transform point from world space to local space.
	XMFLOAT3 InverseTransformPoint(XMFLOAT3 worldPoint);

	// Transform direction from local space to world space.
	XMFLOAT3 TransformDirection(XMFLOAT3 localDirection);

	// Transform direction from world space to local space.
	XMFLOAT3 InverseTransformDirection(XMFLOAT3 worldDirection);

	// Get world translation.
	XMFLOAT3 GetTranslation();

	// Get local translation.
	XMFLOAT3 GetLocalTranslation();

	// Set world translation
	void SetTranslation(XMFLOAT3 newPosition);

	// Set local translation
	void SetLocalTranslation(XMFLOAT3 newPosition);

	XMFLOAT3X3 GetRotation();

	XMFLOAT3X3 GetLocalRotation();

	void SetLocalRotation(XMFLOAT3 newEulerAngles);

	void SetLocalRotation(XMFLOAT3 axis, float newAngle);
	
	void SetLocalRotation(XMFLOAT3X3 newRotation);

	bool IsUniformScale();

	XMFLOAT3 GetScale();

	XMFLOAT3 GetLocalScale();

	// Set local scale
	void SetLocalScale(XMFLOAT3 newScale);

	XMFLOAT3 GetForward();

	XMFLOAT3 GetRight();

	XMFLOAT3 GetUp();

	// Concatenate translation, scale, and rotation in to local and world matrices.
	void UpdateLocalAndWorld();

private:
	void ApplyRotation(XMMATRIX* rotation);
};