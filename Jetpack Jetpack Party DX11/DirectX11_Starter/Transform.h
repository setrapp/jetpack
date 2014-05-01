#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <d3d11.h>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <memory>
#include <memory.h>
#include <xmemory>
#include <map>
#include <stdio.h>
#include <xkeycheck.h>
#include <Xinput.h>
#include <d3d11.h>
#include <string>
#include <string.h>
#include <DirectXMath.h>
#include "XController.h"
#include <stdlib.h>
#include <memory>
#include <memory.h>
#include <xmemory>
#include <map>

#ifndef PI
#define PI 3.14159265358979323846f
#endif

using namespace DirectX;
using namespace std;

///////////////////////////////////////////////////
// NOTE: World and Local Matrices are Transposed //
///////////////////////////////////////////////////

class Entity;

class Transform
{
public:
	Entity* entity;
	vector<Transform*>children;

private:
	XMFLOAT3 right, up, forward;
	XMFLOAT4X4 worldMatrix;
	XMFLOAT4X4 localMatrix;
	XMFLOAT3 translation;
	XMFLOAT3X3 rotation;
	XMFLOAT3 scale;
	Transform* parent;
	
public:
	Transform::Transform();

	Transform::~Transform();

	// Translate locally.
	void Transform::Translate(const XMFLOAT3 translation);

	// Rotate locally.
	void Transform::Rotate(const XMFLOAT3 rotation);

	// Rotate locally.
	void Transform::Rotate(const XMFLOAT3X3 rotation);

	// Scale locally.
	void Transform::Scale(const XMFLOAT3 scale);

	void Transform::LookAt(XMFLOAT3 eye, XMFLOAT3 lookAt, XMFLOAT3 up);

	XMFLOAT4X4 Transform::GetWorldMatrix() const;

	XMFLOAT4X4 Transform::GetLocalMatrix() const;

	Transform* Transform::GetParent() const;

	void Transform::SetParent(Transform* parent);

	// Transform point from local space to world space.
	XMFLOAT3 TransformPoint(XMFLOAT3 localPoint) const;

	// Transform point from world space to local space.
	XMFLOAT3 InverseTransformPoint(XMFLOAT3 worldPoint) const;

	// Transform direction from local space to world space.
	XMFLOAT3 TransformDirection(XMFLOAT3 localDirection) const;

	// Transform direction from world space to local space.
	XMFLOAT3 InverseTransformDirection(const XMFLOAT3 worldDirection) const;

	XMFLOAT3 ClampVector(XMFLOAT3* vector, float maxMagnitude, float minMagnitude) const;

	XMFLOAT3 RotationToEuler(XMFLOAT3X3 const* rotationMatrix) const;

	// Get world translation.
	XMFLOAT3 GetTranslation() const;

	// Get local translation.
	XMFLOAT3 GetLocalTranslation() const;

	// Set world translation
	void SetTranslation(const XMFLOAT3 newPosition);

	// Set local translation
	void SetLocalTranslation(const XMFLOAT3 newPosition);

	XMFLOAT3X3 GetRotation() const;

	XMFLOAT3 GetEulerAngles() const;

	XMFLOAT3X3 GetLocalRotation() const;

	XMFLOAT3 GetLocalEulerAngles() const;

	void SetLocalRotation(XMFLOAT3 newEulerAngles);

	void SetLocalRotation(const XMFLOAT3 axis, const float newAngle);
	
	void SetLocalRotation(const XMFLOAT3X3 newRotation);

	bool IsUniformScale() const;

	XMFLOAT3 GetScale() const;

	XMFLOAT3 GetLocalScale() const;

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



//XINPUT_GAMEPAD_DPAD_UP	 0x0001
//XINPUT_GAMEPAD_DPAD_DOWN	 0x0002
//XINPUT_GAMEPAD_DPAD_LEFT	 0x0004
//XINPUT_GAMEPAD_DPAD_RIGHT	 0x0008
//XINPUT_GAMEPAD_START	 0x0010
//XINPUT_GAMEPAD_BACK	 0x0020
//XINPUT_GAMEPAD_LEFT_THUMB	 0x0040
//XINPUT_GAMEPAD_RIGHT_THUMB	 0x0080
//XINPUT_GAMEPAD_LEFT_SHOULDER	 0x0100
//XINPUT_GAMEPAD_RIGHT_SHOULDER	 0x0200
//XINPUT_GAMEPAD_A	 0x1000
//XINPUT_GAMEPAD_B	 0x2000
//XINPUT_GAMEPAD_X	 0x4000
//XINPUT_GAMEPAD_Y	 0x8000