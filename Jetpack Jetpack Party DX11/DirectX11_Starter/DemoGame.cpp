// ----------------------------------------------------------------------------
//  A few notes on project settings
//
//  - The project is set to use the UNICODE character set
//    - This was changed in Project Properties > Config Properties > General > Character Set
//    - This basically adds a "#define UNICODE" to the project
//
//  - The include directories were automagically correct, since the DirectX 
//    headers and libs are part of the windows SDK
//    - For instance, $(WindowsSDK_IncludePath) is set as a project include 
//      path by default.  That's where the DirectX headers are located.
//
//  - Two libraries had to be manually added to the Linker Input Dependencies
//    - d3d11.lib
//    - d3dcompiler.lib
//    - This was changed in Project Properties > Config Properties > Linker > Input > Additional Dependencies
//
//  - The Working Directory was changed to match the actual .exe's 
//    output directory, since we need to load the shader files at run time
//    - This was changed in Project Properties > Config Properties > Debugging > Working Directory
//
// ----------------------------------------------------------------------------

#include <Windows.h>
#include <d3dcompiler.h>
#include "DemoGame.h"

#include "Common.h"
#include <stdio.h>
#include <stdlib.h>

#pragma region Win32 Entry Point (WinMain)

// Win32 Entry Point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
				   PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	// Make the game, initialize and run
	DemoGame game(hInstance);
	
	if( !game.Init() )
		return 0;	
	
	return game.Run();
}

#pragma endregion

#pragma region Constructor / Destructor

DemoGame::DemoGame(HINSTANCE hInstance) : DXGame(hInstance)
{
	windowCaption = L"Demo DX11 Game";
	windowWidth = 800;
	windowHeight = 600;
	currentState = GameState::Started;
	menu = new Menu(device, deviceContext);
	camera = new Camera();
	//sfx = new Sfx();
}

DemoGame::~DemoGame()
{
	ReleaseMacro(vertexShader);
	ReleaseMacro(pixelShader);
	ReleaseMacro(vsConstantBuffer);
	ReleaseMacro(inputLayout);
}

#pragma endregion

#pragma region Initialization

// Initializes the base class (including the window and D3D),
// sets up our geometry and loads the shaders (among other things)
bool DemoGame::Init()
{
	if( !DXGame::Init() )
		return false;

	FLOAT color[4] = {0.4f, 0.6f, 0.75f, 0.0f};

	AssetManager* assetManager = new AssetManager();

	spriteRenderer = new SpriteRenderer(deviceContext);
	spriteRenderer->SetColor(color);	
	fontRenderer = new FontRenderer(device, L"../Assets/font.spritefont");	
	fontRenderer->setSpriteBatch(spriteRenderer->GetSpriteBatch());

	// Set up buffers and such
	CreateGeometryBuffers();
	LoadShadersAndInputLayout();
	this->deltaTime = 0;

	XMVECTOR position	= XMVectorSet(0, 0, -5, 0);
	XMVECTOR target		= XMVectorSet(0, 0, 0, 0);
	XMVECTOR up			= XMVectorSet(0, 1, 0, 0);
	XMMATRIX V			= XMMatrixLookAtLH(position, target, up);

	XMStoreFloat4x4(&camera->view, XMMatrixTranspose(V));

	XMMATRIX W = XMMatrixIdentity();
	for( Entity* e : entities)
		XMStoreFloat4x4(&e->GetWorldMatrix(), XMMatrixTranspose(W));	

	return true;
}


void DemoGame::CreateGeometryBuffers()
{
	XMFLOAT4 red	= XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4 green	= XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	XMFLOAT4 blue	= XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
	XMFLOAT4 mid	= XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	
	Vertex vertices[] = 
	{
		{ XMFLOAT3(+1.0f, +1.0f, +0.0f), red, XMFLOAT2(0, 0) },
		{ XMFLOAT3(-1.0f, -1.0f, +0.0f), green, XMFLOAT2(1, 1) },
		{ XMFLOAT3(+1.0f, -1.0f, +0.0f), blue, XMFLOAT2(0, 1) },		
		{ XMFLOAT3(-1.0f, +1.0f, +0.0f), mid, XMFLOAT2(1, 0) },
	};

	UINT indices[] = { 0, 2, 1, 3, 0, 1 };	

	
	for(int i = 0 ; i < 5; i ++)
	{	
		Entity* entity = new Entity();

		for(Vertex v : vertices)
		{
			int w;
			if(rand() < 500)
				w = -1;
			else
				w = 1;
			v.Position.x += w * rand() % 2;
			v.Position.y += w * rand() % 2;
			v.Position.z += w * rand() %2;
			v.Color.x += rand() % 5;
			v.Color.y += rand() % 5;
			v.Color.z += rand() % 5;
		}
		entity->AddQuad(vertices, indices);
		//if(rand() % 10 < 5)
			//entity->LoadTexture(L"tex1.jpg", device, deviceContext);
		entities.push_back(entity);
	}

}

// Loads shaders from compiled shader object (.cso) files, and uses the
// vertex shader to create an input layout which is needed when sending
// vertex data to the device
void DemoGame::LoadShadersAndInputLayout()
{
	// Set up the vertex layout description
	// This has to match the vertex input layout in the vertex shader
	// We can't set up the input layout yet since we need the actual vert shader
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,	0, 0,	D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12,	D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	// Load Vertex Shader --------------------------------------
	AssetManager* a = AssetManager::Instance();
	vertexShader = AssetManager::Instance()->CreateAndStoreVertexShader("SimpleVertexShader.cso", vertexDesc, ARRAYSIZE(vertexDesc), &inputLayout);

	// Load Pixel Shaders ---------------------------------------
	pixelShader = AssetManager::Instance()->CreateAndStorePixelShader("SimplePixelShader.cso");
	texturePixelShader = AssetManager::Instance()->CreateAndStorePixelShader("TexturePixelShader.cso", "texture");

	// Constant buffers ----------------------------------------
	D3D11_BUFFER_DESC cBufferDesc;
	cBufferDesc.ByteWidth			= sizeof(vsConstantBufferData);
	cBufferDesc.Usage				= D3D11_USAGE_DEFAULT;
	cBufferDesc.BindFlags			= D3D11_BIND_CONSTANT_BUFFER;
	cBufferDesc.CPUAccessFlags		= 0;
	cBufferDesc.MiscFlags			= 0;
	cBufferDesc.StructureByteStride = 0;
	HR(device->CreateBuffer(
		&cBufferDesc,
		NULL,
		&vsConstantBuffer));

	
	menu->setRenderers(fontRenderer);

#ifdef OPTIMIZATION
	deviceContext->IASetInputLayout(inputLayout);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	// Set the current vertex and pixel shaders, as well the constant buffer for the vert shader
	deviceContext->VSSetShader(vertexShader, NULL, 0);
	deviceContext->VSSetConstantBuffers(
		0,	// Corresponds to the constant buffer's register in the vertex shader
		1, 
		&vsConstantBuffer);
	deviceContext->PSSetShader(pixelShader, NULL, 0);
#endif
}

#pragma endregion

#pragma region Window Resizing
// Handles resizing the window and updating our projection matrix to match
void DemoGame::OnResize()
{
	DXGame::OnResize();
		XMMATRIX P = XMMatrixPerspectiveFovLH(
			0.25f * 3.1415926535f,
			AspectRatio(),
			0.1f,
			100.0f);
	
	XMStoreFloat4x4(&camera->projection, XMMatrixTranspose(P));
}
#pragma endregion

#pragma region Game Loop

// Updates the local constant buffer and 
// push it to the buffer on the device
float x = 0;
XMFLOAT3 trans = XMFLOAT3(0, 0, 0);
void DemoGame::UpdateScene(float dt)
{
	//sfx->Update(dt);
	if(currentState == GameState::Playing)
	{
	this->deltaTime = dt;
	
	x = x + dt;
	//entity.Rotate(XMFLOAT3(0, 0, x));
	dt *= 5;
	/*if(GetAsyncKeyState(VK_UP))
	{
		trans.y += dt;
	}
	if(GetAsyncKeyState(VK_DOWN))
	{
		trans.y -= dt;
	}
	if(GetAsyncKeyState(VK_LEFT))
	{
		trans.x -= dt;
	}
	if(GetAsyncKeyState(VK_RIGHT))
	{
		trans.x += dt;
	}	*/
	for(Entity* e: entities)
		{
			auto p = rand() % 2;
			p++;
			if(p <4 )
				e->transform->Translate(XMFLOAT3(rand() % p * 0.1f, rand() % p * 0.1f, rand() % p * 0.1f));
			else
				e->transform->Rotate(XMFLOAT3(rand() % p * 0.1f, rand() % p * 0.1f, rand() % p * 0.1f));

		
			e->Update(dt);
		}
	}

	camera->Update(dt, &vsConstantBufferData);	

	if(currentState == GameState::Started)
	{
		currentState = menu->Update(dt);
	}


	deviceContext->UpdateSubresource(
	vsConstantBuffer,
	0,			
	NULL,		
	&vsConstantBufferData,
	0,
	0);
}

// Clear the screen, redraw everything, present
void DemoGame::DrawScene()
{
	spriteRenderer->ClearScreen(deviceContext, renderTargetView, depthStencilView);
	FLOAT color[4] = {0.4f, 0.6f, 0.75f, 0.0f};
	deviceContext->ClearRenderTargetView(
		renderTargetView,
		color);

	deviceContext->ClearDepthStencilView(
		depthStencilView, 
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);
	if(currentState == GameState::Started)
	{	
		spriteRenderer->Begin();
		menu->Render(deviceContext);
		spriteRenderer->End();
	}

#ifndef OPTIMIZATION
	deviceContext->IASetInputLayout(inputLayout);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	deviceContext->VSSetShader(
		vertexShader, 
		NULL, 
		0);

	deviceContext->VSSetConstantBuffers(
		0,	
		1, 
		&vsConstantBuffer);

	deviceContext->PSSetShader(
		texturePixelShader, 
		NULL, 
		0);
#endif
	for(Entity* e :entities) 
	{
		vsConstantBufferData.world = e->transform->worldMatrix;
		e->Draw();
	}

	HR(swapChain->Present(0, 0));
}

#pragma endregion

#pragma region Mouse Input

// These methods don't do much currently, but can be used for mouse-related input

void DemoGame::OnMouseDown(WPARAM btnState, int x, int y)
{
	prevMousePos.x = x;
	prevMousePos.y = y;
	menu->ProcessMouseInput(btnState, x, y);
	SetCapture(hMainWnd);
}

void DemoGame::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void DemoGame::OnMouseMove(WPARAM btnState, int x, int y)
{
	prevMousePos.x = x;
	prevMousePos.y = y;
}

void DemoGame::OnMouseWheel(WPARAM btnState, int x, int y)
{
	float rot = (float)GET_WHEEL_DELTA_WPARAM(btnState);
	if (rot <= 0)
		rot = +0.085f;
	else
		rot = -0.085f;

	trans.z += rot;
}
#pragma endregion