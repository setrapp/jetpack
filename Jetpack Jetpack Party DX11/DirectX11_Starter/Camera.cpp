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
	XMStoreFloat4x4(&vsConstantBufferdata->view, XMMatrixInverse(nullptr, XMLoadFloat4x4(&transform->GetWorldMatrix())));
	vsConstantBufferdata->projection = projection;
}