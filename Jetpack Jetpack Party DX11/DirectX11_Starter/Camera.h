#ifndef _CAMCOMPONENTS_H
#define _CAMCOMPONENTS_H
#include<d3d11.h>
#include<DirectXMath.h>
using namespace DirectX;
class CameraComponents	
{	
public:
	float depth;
public:
	CameraComponents::XMFLOAT4X4 view;				//size = 16x
	CameraComponents::XMFLOAT4X4 projection;		//size = 16x
};
#endif

#ifndef _CAMERA_H_
#define _CAMERA_H_
#include<d3d11.h>
#include<DirectXMath.h>
#include"Transform.h"
#include"Common.h"
class Camera: public CameraComponents
{
public: 
	Camera();
	void LookAt(XMFLOAT3 eye, XMFLOAT3 lookAt, XMFLOAT3 up);
	virtual void Update(float dt, VertexShaderModelConstantBuffer* vsConstantBufferdata);

public:
	Transform transform;

private:
	XMFLOAT4X4 oldWorldMatrix;

}; 
#endif
