#include "Camera.h"

Camera::Camera()
{		
	transform = new Transform();
	oldWorldMatrix = transform->GetWorldMatrix();
}

void Camera::LookAt(XMFLOAT3 eye, XMFLOAT3 lookAt, XMFLOAT3 up)
{
	transform->LookAt(eye, lookAt, up);
}

void Camera::Update(float dt, VertexShaderModelConstantBuffer* vsConstantBufferdata)
{		
	vsConstantBufferdata->view			= transform->GetWorldMatrix();
	vsConstantBufferdata->projection	= projection;
}