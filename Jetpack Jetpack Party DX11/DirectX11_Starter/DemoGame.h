#pragma once



#include "FontRenderer.h"
#include "Sprite.h"
#include "Vertex.h"
#include "Common.h"
#include <DirectXMath.h>
#include "DXGame.h"
#include "Mesh.h"
#include "Entity.h"
#include "Camera.h"

using namespace DirectX;

// Include run-time memory checking in debug builds
#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

// Demo class which extends the base DXGame class
class DemoGame : public DXGame
{
public:
	DemoGame(HINSTANCE hInstance);
	~DemoGame();

	// Overrides for base level methods
	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene(); 

	// For handing mouse input
	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);
	void OnMouseWheel(WPARAM btnState, int x, int y);
private:
	// Initialization for our "game" demo
	void CreateGeometryBuffers();
	void LoadShadersAndInputLayout();

private:
	float deltaTime;
	// Our basic shaders for this example
	ID3D11PixelShader* pixelShader;
	ID3D11VertexShader* vertexShader;

	// A few more odds and ends we'll need
	ID3D11InputLayout* inputLayout;
	ID3D11Buffer* vsConstantBuffer;
	VertexShaderConstantBuffer vsConstantBufferData;

	// Keeps track of the old mouse position.  Useful for 
	// determining how far the mouse moved in a single frame.
	POINT prevMousePos;
	//Camera* camera;
	Entity* entity;
	SpriteRenderer* spriteRenderer;
	//FontRenderer* fontRenderer;

	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 projectionMatrix;
};