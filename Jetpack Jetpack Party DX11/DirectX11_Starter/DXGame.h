// -------------------------------------------------------------
//  Base class for a DirectX11 "game", heavily borrowing from
//  the structure set up by Frank D. Luna in "3D Game
//  Programming with DirectX 11". (d3dApp.h & d3dApp.cpp)
//
//  This version doesn't rely on D3DX or the Effect framework,
//  as these libraries are deprecated.
// -------------------------------------------------------------

#pragma once

#include "Common.h"
#include "Windows.h"
#include <string>
#include <d3d11.h>
#include <assert.h>

#include "dxerr.h"
#include "GameTimer.h"


class DXGame
{
public:
	DXGame(HINSTANCE hInstance);
	virtual ~DXGame(void);

	// Our window's aspect ratio
	float AspectRatio()const;
	
	// The game loop
	int Run();
 
	// Methods called by the game loop - override these in
	// derived classes to implement custom functionality
	virtual bool Init();
	virtual void OnResize(); 
	virtual void UpdateScene(float dt)=0;
	virtual void DrawScene()=0; 
	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	// Convenience overrides for handling mouse input.
	virtual void OnMouseDown(WPARAM btnState, int x, int y){ }
	virtual void OnMouseUp(WPARAM btnState, int x, int y)  { }
	virtual void OnMouseMove(WPARAM btnState, int x, int y){ }	
	virtual void OnMouseWheel(WPARAM btnState, int x, int y) { }

protected:
	// Handles window and Direct3D initialization
	bool InitMainWindow();
	bool InitDirect3D();

	void CalculateFrameStats();

protected:

	// Window handles and such
	HINSTANCE hAppInst;
	HWND      hMainWnd;

	// Game and window state tracking
	bool      gamePaused;
	bool      minimized;
	bool      maximized;
	bool      resizing;

	// Timer for running the game on delta time
	GameTimer timer;
	
	// DirectX related buffers, views, etc.
	UINT msaa4xQuality;
	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;
	IDXGISwapChain* swapChain;
	ID3D11Texture2D* depthStencilBuffer;
	ID3D11RenderTargetView* renderTargetView;
	ID3D11DepthStencilView* depthStencilView;
	D3D11_VIEWPORT viewport;
	D3D_DRIVER_TYPE driverType;
	D3D_FEATURE_LEVEL featureLevel;

	// Derived class can set these in derived constructor to customize starting values.
	std::wstring windowCaption;
	int windowWidth;
	int windowHeight;
	bool enable4xMsaa;
};

