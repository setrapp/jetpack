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
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <d3dcompiler.h>
#include "DemoGame.h"
#include "Common.h"
#include <stdio.h>
#include <stdlib.h>
#include <comdef.h>
#include <iostream>
#include "Player.h"
#include "Debug.h"
#include "XInputValues.h"
#include "InputManager.h"
#include "SoundManager.h"
#include "SpriteRenderer.h"
#include "HUD.h"
#include "Skybox.h"

using namespace std;
InputManager* IPMan::inputManager = NULL;
Player* player = NULL;

#pragma region Win32 Entry Point (WinMain)

// Win32 Entry Point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
				   PSTR cmdLine, int showCmd)
{
#if defined(DEBUG) | defined(_DEBUG)	// Enable run-time memory check for debug builds.

	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	// Make the game, initialize and run
	DemoGame game(hInstance);

	if( !game.Init() )
		return 0;	
	int a = 0;

	return game.Run();
}

#pragma endregion

#pragma region Constructor / Destructor

DemoGame::DemoGame(HINSTANCE hInstance) : DXGame(hInstance)
{
	flag = true;
	windowCaption = L"Jetpack Jetpack Party!";
	windowWidth = 1920;
	windowHeight = 1080;
	currentState = GameState::MenuState;
	playerCamera = new Camera();
	debugCamera = new ControllableCamera();
	camera = playerCamera;
	light = new Light(XMFLOAT3(0, -1, 1), XMFLOAT4(1, 1, 1, 1), XMFLOAT4(1, 1, 1, 1), XMFLOAT4(1, 1, 1, 1), true);
	mouseCursorVisibility = true;
	mouseLook = NULL;
}

		
	

DemoGame::~DemoGame()
{
	ReleaseMacro(vsModelConstantBuffer);
	ReleaseMacro(materialsAndLightsConstantBuffer);
	ReleaseMacro(deferredDepthlessState);
	
	delete deferredRenderer;
	delete deferredPlane;
	
	for(int i = 0 ; i < entities.size(); i++)
	{
		delete entities.at(i);
	}

	delete IPMan::GetIPMan();
	delete assetManager;
	delete FontManager::Instance();

	delete m_hud;
	delete light;
	delete camera;
	delete menu;
	delete mouseLook;
	delete spriteRenderer;
}

#pragma endregion

#pragma region Initialization

float clearColor[4] = {0.4f, 0.6f, 0.75f, 0.0f};

bool DemoGame::Init()
{
	
	if( !DXGame::Init() )
		return false;

	// Set Cull Mode.
	ID3D11RasterizerState* rasterizerState;
	D3D11_RASTERIZER_DESC rasterizerDesc;
	deviceContext->RSGetState(&rasterizerState);
	if (rasterizerState)
	{
		rasterizerState->GetDesc(&rasterizerDesc);
	}
	else
	{
		ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));
		rasterizerDesc.CullMode = D3D11_CULL_BACK;
		rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	}
	HRESULT hr = device->CreateRasterizerState(&rasterizerDesc, &rasterizerState);
	if (FAILED(hr))
		int a = 0;
	deviceContext->RSSetState(rasterizerState);
	ReleaseMacro(rasterizerState);

	assetManager = new AssetManager();

	// Create depthless state for rendering to deferred plane.
	D3D11_DEPTH_STENCIL_DESC deferredDepthlessDesc;
	ZeroMemory(&deferredDepthlessDesc, sizeof(deferredDepthlessDesc));
	deviceContext->OMGetDepthStencilState(&deferredDepthlessState, NULL);
	if (deferredDepthlessState)
	{
		deferredDepthlessState->GetDesc(&deferredDepthlessDesc);
	}
	deferredDepthlessDesc.DepthEnable = false;
	device->CreateDepthStencilState(&deferredDepthlessDesc, &deferredDepthlessState);

	spriteRenderer = new SpriteRenderer(deviceContext);
	RECT rect;
	GetClientRect(GetActiveWindow(), &rect);	
	menu = new Menu(FontManager::Instance()->AddFont("MENUFONT", device, spriteRenderer, L"../Assets/Fonts/font.spritefont"), spriteRenderer, rect.left + rect.right, rect.top + rect.bottom );	

	LoadShadersAndInputLayout();

	AssetManager::Instance()->StoreMaterial(new Material());

	XMFLOAT3 cameraPosition;
	XMStoreFloat3(&cameraPosition, XMVectorSet(0, 0, -50, 0));
	XMFLOAT3 cameraTarget;
	XMStoreFloat3(&cameraTarget, XMVectorSet(0, 0, 0, 0));
	XMFLOAT3 cameraUp;
	XMStoreFloat3(&cameraUp, XMVectorSet(0, 1, 0, 0));

	camera->LookAt(cameraPosition, cameraTarget, cameraUp);
	XMStoreFloat4x4(&deferredView, XMMatrixTranspose(XMMatrixLookAtLH(XMLoadFloat3(&cameraPosition), XMLoadFloat3(&cameraTarget), XMLoadFloat3(&cameraUp))));

	// Set up buffers and such
	CreateGeometryBuffers();
	this->deltaTime = 0;
	
	LoadSoundAssets();

	input = new IPMan(INPUTMODES::KEYBOARD);

	deferredRenderer = new DeferredRenderer(windowWidth, windowHeight);
	mouseLook = new MouseLook(NULL, XMFLOAT2(0.01f, 0.01f));

	for (int i = 0; i < TARGET_COUNT; i++)
	{
		nullRenderTargets[i] = NULL;
	}
	for (int i = 0; i < TARGET_COUNT; i++)
	{
		nullShaderResources[i] = NULL;
	}

	return true;
}

void DemoGame::CreateGeometryBuffers()
{
	XMFLOAT4 red	= XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4 green	= XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	XMFLOAT4 blue	= XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
	XMFLOAT4 mid	= XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);

	AssetManager::Instance()->CreateAndStoreModel("../Assets/Models/cube.obj");
	AssetManager::Instance()->CreateAndStoreModel("../Assets/Models/BasicJetMan.obj", "jetman");
	AssetManager::Instance()->CreateAndStoreModel("../Assets/Models/Fireball.obj", "fireball");
	AssetManager::Instance()->CreateAndStoreModel("../Assets/Models/BasicTrack.obj", "terrain");
	AssetManager::Instance()->CreateAndStoreModel("../Assets/Models/BasicTrackNav.obj", "terrain_nav");

	// Create orthographic and projection plane for deferred rendering.
	float halfWindowWidth = windowWidth / 2, halfWindowHieght= windowHeight / 2;
	XMStoreFloat4x4(&deferredView, XMMatrixTranspose(XMMatrixLookAtLH(XMLoadFloat3(&XMFLOAT3(0, 0, -50)), XMLoadFloat3(&XMFLOAT3(0, 0, 0)), XMLoadFloat3(&XMFLOAT3(0, 1, 0)))));
	XMStoreFloat4x4(&deferredProjection, XMMatrixTranspose(XMMatrixOrthographicLH(windowWidth, windowHeight, 0.1f, 100.0f)));
	Vertex deferredVertices[] = 
	{
		{ XMFLOAT3(+halfWindowWidth, +halfWindowHieght, +0.0f), XMFLOAT3(0, 0, -1), XMFLOAT2(1, 0) },
		{ XMFLOAT3(-halfWindowWidth, -halfWindowHieght, +0.0f), XMFLOAT3(0, 0, -1), XMFLOAT2(0, 1) },
		{ XMFLOAT3(+halfWindowWidth, -halfWindowHieght, +0.0f), XMFLOAT3(0, 0, -1), XMFLOAT2(1, 1) },		
		{ XMFLOAT3(-halfWindowWidth, +halfWindowHieght, +0.0f), XMFLOAT3(0, 0, -1), XMFLOAT2(0, 0) },
	};
	UINT deferredIndices[] = { 0, 2, 1, 3, 0, 1 };
	AssetManager::Instance()->StoreMaterial(new Material(XMFLOAT4(0.0f, 0.0f, 0.0f, 1), XMFLOAT4(0, 0, 0, 0), XMFLOAT4(0, 0, 0, 0), 16), "deferred");
	deferredPlane = new Entity();
	deferredPlane->AddQuad(deferredVertices, deferredIndices);
	deferredPlane->SetBaseMaterial("deferred");
	deferredPlane->GetBaseMaterial()->pixelShader = AssetManager::Instance()->GetPixelShader("deferred");
	deferredPlane->Finalize();
	
	// Attempt to load model
	player = new Player();
	player->AddModel(AssetManager::Instance()->GetModel());
	player->Finalize();
	entities.push_back(player);
	player->transform.Translate(XMFLOAT3(0, 1000, 0));
	player->respawnPosition = player->transform.GetTranslation();
	AttachCameraToPlayer();
	
	Entity* jetman = new Entity();
	jetman->AddModel(AssetManager::Instance()->GetModel("jetman"));
	jetman->Finalize();
	jetman->transform.Rotate(XMFLOAT3(0, PI / 2, 0));
	entities.push_back(jetman);
	jetman->transform.SetParent(&player->transform);
	jetman->transform.SetLocalTranslation(XMFLOAT3(0, 0, 0));
	jetman->transform.Translate(XMFLOAT3(0, -5, 0));
	
	if (player->jetpack->thrusterActives)
	{
		for (int i = 0; i < player->jetpack->thrusterCount; i++)
		{
			if (player->jetpack->thrusters[i])
			{
				entities.push_back(player->jetpack->thrusters[i]);
			}
		}
	}
	

	Vertex vertices[] = 
	{
		{ XMFLOAT3(+1.0f, +1.0f, +0.0f), XMFLOAT3(0, 0, -1), XMFLOAT2(0, 0) },
		{ XMFLOAT3(-1.0f, -1.0f, +0.0f), XMFLOAT3(0, 0, -1), XMFLOAT2(1, 1) },
		{ XMFLOAT3(+1.0f, -1.0f, +0.0f), XMFLOAT3(0, 0, -1), XMFLOAT2(0, 1) },		
		{ XMFLOAT3(-1.0f, +1.0f, +0.0f), XMFLOAT3(0, 0, -1), XMFLOAT2(1, 0) },
	};

	UINT indices[] = { 0, 2, 1, 3, 0, 1 };

	Vertex hudVertices[] = 
	{
		{ XMFLOAT3(+1.0f, +1.0f, +0.0f), XMFLOAT3(0, 0, -1), XMFLOAT2(0, 0) },
		{ XMFLOAT3(-1.0f, -1.0f, +0.0f), XMFLOAT3(0, 0, -1), XMFLOAT2(1, 1) },
		{ XMFLOAT3(+1.0f, -1.0f, +0.0f), XMFLOAT3(0, 0, -1), XMFLOAT2(0, 1) },	
	};
		
	UINT hudIndices[] = { 0, 2, 1};

	m_hud = new HUD(spriteRenderer, FontManager::Instance()->GetFont("MENUFONT"));
	

	Entity* gift = new Entity();
	gift->AddQuad(vertices, indices);
	
	gift->transform.SetTranslation(player->targetPosition);
	entities.push_back(gift);
	AssetManager::Instance()->StoreMaterial(new Material(XMFLOAT4(0.3f, 0.3f, 0.3f, 1), XMFLOAT4(1, 1, 1, 1), XMFLOAT4(1, 1, 1, 1), 16), "gift");
	gift->SetBaseMaterial("gift");
	gift->GetBaseMaterial()->pixelShader = AssetManager::Instance()->GetPixelShader("texture");
	gift->LoadTexture(L"../Assets/Textures/RedGift.png");
	gift->Finalize();

	Entity* floor = new Entity();
	floor->AddModel(AssetManager::Instance()->GetModel("terrain"));
	floor->transform.Translate(XMFLOAT3(0, -400, 0));
	floor->transform.Scale(XMFLOAT3(1000, 1000, 1000));
	entities.push_back(floor);
	floor->Finalize();

	Entity* navMesh = new Entity();
	navMesh->AddModel(AssetManager::Instance()->GetModel("terrain_nav"));
	navMesh->transform.SetTranslation(floor->transform.GetTranslation());
	navMesh->transform.SetLocalScale(floor->transform.GetScale());
	//entities.push_back(navMesh);
	navMesh->Finalize();
}

#pragma endregion

#pragma region Load Content
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
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	// Load Vertex Shaders --------------------------------------
	vertexShader = AssetManager::Instance()->CreateAndStoreVertexShader("../Debug/SimpleVertexShader.cso", vertexDesc, ARRAYSIZE(vertexDesc), &inputLayout);
	
	// Load Pixel Shaders ---------------------------------------
	pixelShader = AssetManager::Instance()->CreateAndStorePixelShader("../Debug/SimplePixelShader.cso");
	texturePixelShader = AssetManager::Instance()->CreateAndStorePixelShader("../Debug/TexturePixelShader.cso", "texture");
	texturePixelShader = AssetManager::Instance()->CreateAndStorePixelShader("../Debug/DeferredPixelShader.cso", "deferred");

	// Constant buffers ----------------------------------------
	// Vertex Shader Per Model Constant Buffer
	D3D11_BUFFER_DESC cBufferDesc;
	cBufferDesc.ByteWidth			= sizeof(vsModelConstantBufferData);
	cBufferDesc.Usage				= D3D11_USAGE_DEFAULT;
	cBufferDesc.BindFlags			= D3D11_BIND_CONSTANT_BUFFER;
	cBufferDesc.CPUAccessFlags		= 0;
	cBufferDesc.MiscFlags			= 0;
	cBufferDesc.StructureByteStride = 0;
	HR(device->CreateBuffer(
		&cBufferDesc,
		NULL,
		&vsModelConstantBuffer));

	// Light Constant Buffer
	cBufferDesc.ByteWidth			= sizeof(materialsAndLightsConstantBufferData);
	cBufferDesc.Usage				= D3D11_USAGE_DEFAULT;
	cBufferDesc.BindFlags			= D3D11_BIND_CONSTANT_BUFFER;
	cBufferDesc.CPUAccessFlags		= 0;
	cBufferDesc.MiscFlags			= 0;
	cBufferDesc.StructureByteStride = 0;
	HR(device->CreateBuffer(
		&cBufferDesc,
		NULL,
		&materialsAndLightsConstantBuffer));
}

void DemoGame::LoadSoundAssets()
{
	SoundId id;
	id = SoundId::SAMPLEBG;
	assetManager->Instance()->GetSoundManager()->LoadSound(id, L"../Assets/Sounds/SampleBG.wav");
	id = SoundId::SINK;
	assetManager->Instance()->GetSoundManager()->LoadSound(id, L"../Assets/Sounds/Sunk.wav");
	assetManager->Instance()->GetSoundManager()->PlaySoundInstance(SoundId::SAMPLEBG, true, true);
	assetManager->Instance()->GetSoundManager()->PlaySoundInstance(SoundId::SINK);
	#ifdef _DEBUG
    assetManager->Instance()->GetSoundManager()->Mute(true);
	#endif
}

#pragma endregion

#pragma region Window Focus
void DemoGame::OnFocus(bool givenFocus)
{
	DXGame::OnFocus(givenFocus);
	if (mouseLook)
	{
		if (givenFocus)
		{
			mouseLook->ResetCursor();
			mouseLook->ignoreMouse = false;
		}
		else
		{
			mouseLook->ignoreMouse = true;
		}
	}
}
#pragma endregion Window Focus

#pragma region Window Resizing
// Handles resizing the window and updating our projection matrix to match
void DemoGame::OnResize()
{
	DXGame::OnResize();
	XMMATRIX P = XMMatrixPerspectiveFovLH(
		0.25f * 3.1415926535f,
		AspectRatio(),
		0.1f,
		100000.0f);

	XMStoreFloat4x4(&playerCamera->projection, XMMatrixTranspose(P));
	XMStoreFloat4x4(&debugCamera->projection, XMMatrixTranspose(P));

	if (mouseLook)
	{
		mouseLook->ResetCursor();
	}

	if(currentState == GameState::MenuState)
	{
		if(menu)
			menu->WindowResize();
	}

	if(m_hud)
		if(currentState == GameState::Playing)
			m_hud->Reset();
}
#pragma endregion

#pragma region Game Loop

// Update the scene.
void DemoGame::UpdateScene(float dt)
{	
	(IPMan::GetIPMan()->GetAllKeys());
	if (IPMan::GetIPMan()->GetBack())
	{
		currentState = Helper::GoBackOnce(currentState);
#ifndef _PAUSEMENU
		if(currentState == GameState::Paused)
			currentState = GameState::MenuState;
#endif
	}

	assetManager->Instance()->GetSoundManager()->Update();
	if(currentState == GameState::Playing)
	{
		this->deltaTime = dt;

		while (!AssetManager::Instance()->addedEntities.empty())
		{
			entities.push_back(AssetManager::Instance()->addedEntities.front());
			AssetManager::Instance()->addedEntities.pop();
		}


		for(Entity* e: entities)
		{
			e->Update(dt);
		}
	}

	if(camera != debugCamera)
	{
		XMFLOAT3 debugEye = camera->transform.GetTranslation();
		XMFLOAT3 debugTarget;
		XMStoreFloat3(&debugTarget, XMVectorAdd(XMLoadFloat3(&camera->transform.GetTranslation()), XMLoadFloat3(&camera->transform.GetForward())));
		XMFLOAT3 debugUp = camera->transform.GetUp();
		debugCamera->LookAt(debugEye, debugTarget, debugUp);
	}

	camera->Update(dt, &vsModelConstantBufferData);	

	if(currentState == GameState::MenuState)
	{
		GameState newState = menu->Update(dt);
		if (currentState != newState)
		{
			mouseLook->ResetCursor();
			currentState = newState;
		}
	}


	deviceContext->UpdateSubresource(
	vsModelConstantBuffer,
	0,			
	NULL,		
	&vsModelConstantBufferData,
	0,
	0);
}

// Clear the screen, redraw everything, present
void DemoGame::DrawScene()
{
	// Prepare for rendering to textures.
	deviceContext->PSSetShaderResources(0, TARGET_COUNT, nullShaderResources);
	deferredRenderer->SetTargets();
	deferredRenderer->ClearTargets(clearColor);

#ifndef OPTIMIZATION
	deviceContext->IASetInputLayout(inputLayout);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	deviceContext->VSSetConstantBuffers(
		0,	
		1, 
		&vsModelConstantBuffer);
#endif

	if (currentState == GameState::Playing) {		
		if(mouseCursorVisibility)
		{
			mouseCursorVisibility = false;
			ShowCursor(mouseCursorVisibility);
		}
		// Update light constant buffer for vertex and pixel shader.
		materialsAndLightsConstantBufferData.light = light->GetShaderLight();
		materialsAndLightsConstantBufferData.material = AssetManager::Instance()->GetMaterial("default")->GetShaderMaterial();
		DXConnection::Instance()->deviceContext->UpdateSubresource(materialsAndLightsConstantBuffer, 0, NULL, &materialsAndLightsConstantBufferData, 0, 0);
		DXConnection::Instance()->deviceContext->VSSetConstantBuffers(1, 1, &materialsAndLightsConstantBuffer);
		DXConnection::Instance()->deviceContext->PSSetConstantBuffers(1, 1, &materialsAndLightsConstantBuffer);

		// Store entity drawing arguments.
		EntityDrawArgs entityDrawArgs;
		entityDrawArgs.vsModelConstantBuffer = vsModelConstantBuffer;
		entityDrawArgs.vsModelConstantBufferData = &vsModelConstantBufferData;
		entityDrawArgs.materialsAndLightsConstantBuffer = materialsAndLightsConstantBuffer;
		entityDrawArgs.materialsAndLightsConstantBufferData = &materialsAndLightsConstantBufferData;

		// Draw entities.
		for(Entity* e :entities) 
		{			
			e->Draw(&entityDrawArgs);
		}
		
	}
	flag = true;

	// Prepare render to back buffer.
	ID3D11DepthStencilState* depthStencilState;
	deviceContext->OMGetDepthStencilState(&depthStencilState, NULL);
	deviceContext->OMSetDepthStencilState(deferredDepthlessState, NULL);	
	deviceContext->OMSetRenderTargets(TARGET_COUNT, nullRenderTargets, depthStencilView);
	deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
	deviceContext->ClearRenderTargetView(renderTargetView, clearColor);
	deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	deviceContext->PSSetShaderResources(0, TARGET_COUNT, deferredRenderer->GetShaderResourceViews());

	// Store entity drawing arguments.
	EntityDrawArgs deferredPlaneDrawArgs;
	deferredPlaneDrawArgs.vsModelConstantBuffer = vsModelConstantBuffer;
	deferredPlaneDrawArgs.vsModelConstantBufferData = &vsModelConstantBufferData;
	deferredPlaneDrawArgs.materialsAndLightsConstantBuffer = materialsAndLightsConstantBuffer;
	deferredPlaneDrawArgs.materialsAndLightsConstantBufferData = &materialsAndLightsConstantBufferData;

	// Draw rendering plane to back buffer.
	deferredPlane->Draw(&deferredPlaneDrawArgs, &deferredView, &deferredProjection);

	// Reset to usual 3D rendering settings.
	deviceContext->OMSetDepthStencilState(depthStencilState, NULL);

	// TODO Render transparency here.

	// Render Menus and HUD.
	if(currentState == GameState::MenuState)
	{
		spriteRenderer->Begin();
		menu->Render();
		spriteRenderer->End();
		if(!mouseCursorVisibility)
		{
			mouseCursorVisibility = true;
			ShowCursor(mouseCursorVisibility);
		}
	}

	else
		if(currentState == GameState::Playing)
		{
			m_hud->Render();
		}


	// Present to front buffer.
	HR(swapChain->Present(0, 0));
}


void DemoGame::FixedUpdate() 
{    
}


#pragma endregion

#pragma region Mouse Input

// These methods don't do much currently, but can be used for mouse-related input
void DemoGame::OnMouseDown(WPARAM btnState, int x, int y)
{
	prevMousePos.x = x;
	prevMousePos.y = y;
	SetCapture(hMainWnd);

	// Right mousehold detaches camera
	if(btnState == 2)
	{
		if (camera != debugCamera)
		{	
			debugCamera->controllable = true;
			player->controllable = false;
			camera = debugCamera;
			mouseLook->SetLooker(&debugCamera->transform);
		}
	}
}

void DemoGame::OnMouseUp(WPARAM btnState, int x, int y)
{
	if(camera == debugCamera)
	{
		debugCamera->controllable = false;
		player->controllable = true;
		camera = playerCamera;
		mouseLook->SetLooker(NULL);
	}
	ReleaseCapture();
}


void DemoGame::OnMouseMove(WPARAM btnState, int x, int y)
{
	if(flag)
	{
		flag = false;
		prevMousePos.x = x;
		prevMousePos.y = y;
		if(currentState == Playing && mouseLook)
		{
			mouseLook->MouseMove(btnState, x, y);		
		}
	}
}

void DemoGame::OnMouseWheel(WPARAM btnState, int x, int y)
{
	//float rot = (float)GET_WHEEL_DELTA_WPARAM(btnState);	
}
#pragma endregion

#pragma region CameraAttach
void DemoGame::AttachCameraToPlayer()
{
	playerCamera->transform.SetParent(&player->transform);
	XMFLOAT3 eye;
	XMStoreFloat3(&eye, XMVectorAdd(XMVectorSubtract(XMLoadFloat3(&player->transform.GetTranslation()), (50 * XMLoadFloat3(&player->transform.GetForward()))), (10 * XMLoadFloat3(&player->transform.GetUp()))));
	XMFLOAT3 target;
	XMStoreFloat3(&target, XMVectorAdd(XMLoadFloat3(&player->transform.GetTranslation()), (3 * XMLoadFloat3(&player->transform.GetForward()))));
	XMFLOAT3 up = player->transform.GetUp();
	playerCamera->LookAt(eye, target, up);
}
#pragma endregion