#include <DirectXMath.h>

class Camera
{
public :
	Camera::Camera()
	{
		 DirectX::XMStoreFloat4x4(&this->viewMatrix, DirectX::XMMatrixIdentity());
		 DirectX::XMStoreFloat4x4(&this->projectionMatrix, DirectX::XMMatrixIdentity());
	}
	Camera::~Camera()
	{
	}

	DirectX::XMFLOAT4X4* Camera::getView()
	{
		return &viewMatrix;
	}

	DirectX::XMFLOAT4X4* Camera::getProjection()
	{
		return &projectionMatrix;
	}

	void Camera::Resize(float aspectRatio)
	{
		XMMATRIX P = XMMatrixPerspectiveFovLH(
			0.25f * 3.1415926535f,
			aspectRatio,
			0.1f,
			100.0f);

		XMFLOAT4X4 temp;
		XMStoreFloat4x4(&temp, XMMatrixTranspose(P));
		this->projectionMatrix = temp;
	}

private:
	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 projectionMatrix;
};