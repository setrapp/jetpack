#ifndef _CAMCOMPONENTS_H
#define _CAMCOMPONENTS_H
using namespace DirectX;
class CameraComponents	
{
	
public:
	float depth;
	CameraComponents::CameraComponents()
	{

	}
	~CameraComponents()
	{

	}
public:
	CameraComponents::XMFLOAT4X4 view;			//size = 16x
	CameraComponents::XMFLOAT4X4 projection;		//size = 16x
};
#endif

#ifndef _CAMERA_H_
#define _CAMERA_H_
#include<d3d11.h>
#include<DirectXMath.h>
#include"Transform.h"
#include"Common.h"
class Camera: public CameraComponents, Transform
{
public: 	

	Camera(void)
	{		
	}

	void LookAt(FXMVECTOR lookAt, FXMVECTOR eye, FXMVECTOR up)
	{
		XMMATRIX viewNew = XMMatrixLookAtLH(eye, lookAt, up);
		XMStoreFloat4x4(&view, XMMatrixTranspose(viewNew));
	}

	_inline void Update(float dt, VertexShaderModelConstantBuffer* vsConstantBufferdata)
	{		
		vsConstantBufferdata->view			= view;
		vsConstantBufferdata->projection	= projection;
	}


private:	

}; 
#endif
