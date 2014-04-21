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
	ipMan = new InputManager(INPUTMODES::XCONTROLLER);
	xnew = new XNEW(ipMan->GetXController());
	flag = true;
	windowCaption = L"Jetpack Jetpack Party!";
	windowWidth = 800;
	windowHeight = 600;
	currentState = GameState::Started;
	camera = new ControllableCamera();
	light = new Light(XMFLOAT3(0, -1, 1), XMFLOAT4(1, 1, 1, 1), XMFLOAT4(1, 1, 1, 1), XMFLOAT4(1, 1, 1, 1), true);
	mouseCursorVisibility = true;
}

DemoGame::~DemoGame()
{
	ReleaseMacro(vertexShader);
	ReleaseMacro(pixelShader);
	ReleaseMacro(vsModelConstantBuffer);
	ReleaseMacro(materialsAndLightsConstantBuffer);
	ReleaseMacro(inputLayout);

	delete ipMan;
	delete xnew;

	delete assetManager;

	for(int i = 0 ; i < entities.size(); i++)
	{
		delete entities.at(i);
	}
	entities.clear();
	
	
	delete light;
	delete camera;
	delete menu;
	delete mouseLook;
	delete fontRenderer;
	delete spriteRenderer;
}

#pragma endregion

#pragma region Initialization

float clearColor[4] = {0.4f, 0.6f, 0.75f, 0.0f};

bool DemoGame::Init()
{
	if( !DXGame::Init() )
		return false;

	AssetManager* assetManager = new AssetManager();

	/*spriteRenderer = new SpriteRenderer(deviceContext);
	fontRenderer = new FontRenderer(device, L"../Assets/font.spritefont");	
	fontRenderer->setSpriteBatch(spriteRenderer->GetSpriteBatch());	
	menu = new Menu(fontRenderer);*/

	//LoadShadersAndInputLayout();

	//AssetManager::Instance()->StoreMaterial(new Material());

	/*XMFLOAT3 cameraPosition;
	XMStoreFloat3(&cameraPosition, XMVectorSet(0, 10, -50, 0));
	XMFLOAT3 cameraTarget;
	XMStoreFloat3(&cameraTarget, XMVectorSet(0, 0, 0, 0));
	XMFLOAT3 cameraUp;
	XMStoreFloat3(&cameraUp, XMVectorSet(0, 1, 0, 0));

	camera->LookAt(cameraPosition, cameraTarget, cameraUp);*/

	// Set up buffers and such
	//CreateGeometryBuffers();
	this->deltaTime = 0;
	
	//LoadSoundAssets();

	// Create temporary mouse look
	//mouseLook = new MouseLook(NULL, XMFLOAT2(0, 0));

	return true;
}

void DemoGame::CreateGeometryBuffers()
{
	XMFLOAT4 red	= XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4 green	= XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	XMFLOAT4 blue	= XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
	XMFLOAT4 mid	= XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);

	// Attempt to load model
	AssetManager::Instance()->CreateAndStoreModel("../Assets/video_camera.obj", "camera");
	Player* player = new Player();
	//player->AddModel(AssetManager::Instance()->GetModel("camera"));
	entities.push_back(player);
	//player->Finalize();
	AssetManager::Instance()->StoreMaterial(new Material(XMFLOAT4(0.3, 0.3, 0.3, 1), XMFLOAT4(1, 0, 1, 1), XMFLOAT4(0.5f, 0.5f, 0.5f, 0.5f), 16), "camera");
	player->SetMaterial("camera");
	mouseLook = new MouseLook(&player->transform, XMFLOAT2(0.01f, 0.01f));

	Entity* emptyEntity = new Entity();
	entities.push_back(emptyEntity);

	AssetManager::Instance()->CreateAndStoreModel("../Assets/cube.obj", "cube");
	AssetManager::Instance()->CreateAndStoreModel("../Assets/BasicJetMan.obj", "jetman");
	Entity* cube = new Entity();
	cube->AddModel(AssetManager::Instance()->GetModel("jetman"));
	cube->Finalize();
	cube->transform.Translate(XMFLOAT3(0, -5, 0));
	cube->transform.Rotate(XMFLOAT3(0, PI / 2, 0));
	entities.push_back(cube);
	cube->transform.SetParent(&player->transform);
	//cube->transform->SetParent(&emptyEntity->transform);
	//emptyEntity->transform->SetParent(&player->transform);

	Vertex vertices[] = 
	{
		{ XMFLOAT3(+1.0f, +1.0f, +0.0f), XMFLOAT3(0, 0, -1), XMFLOAT2(0, 0) },
		{ XMFLOAT3(-1.0f, -1.0f, +0.0f), XMFLOAT3(0, 0, -1), XMFLOAT2(1, 1) },
		{ XMFLOAT3(+1.0f, -1.0f, +0.0f), XMFLOAT3(0, 0, -1), XMFLOAT2(0, 1) },		
		{ XMFLOAT3(-1.0f, +1.0f, +0.0f), XMFLOAT3(0, 0, -1), XMFLOAT2(1, 0) },
	};

	UINT indices[] = { 0, 2, 1, 3, 0, 1 };

	Entity* gift = new Entity();
	gift->AddQuad(vertices, indices);
	gift->Finalize();
	gift->transform.Translate(XMFLOAT3(-5, 5, 0));
	entities.push_back(gift);

	AssetManager::Instance()->StoreMaterial(new Material(XMFLOAT4(0.3f, 0.3f, 0.3f, 1), XMFLOAT4(1, 1, 1, 1), XMFLOAT4(1, 1, 1, 1), 16), "gift");
	gift->SetMaterial("gift");
	gift->GetMaterial()->pixelShader = AssetManager::Instance()->GetPixelShader("texture");
	gift->LoadTexture(L"../Assets/RedGift.png");

	Vertex floorVertices[] = 
	{
		{ XMFLOAT3(+100.0f, -10.0f, +100.0f), XMFLOAT3(0, 0, -1), XMFLOAT2(0, 0) },
		{ XMFLOAT3(-100.0f, -10.0f, -100.0f), XMFLOAT3(0, 0, -1), XMFLOAT2(1, 1) },
		{ XMFLOAT3(+100.0f, -10.0f, -100.0f), XMFLOAT3(0, 0, -1), XMFLOAT2(0, 1) },		
		{ XMFLOAT3(-100.0f, -10.0f, +100.0f), XMFLOAT3(0, 0, -1), XMFLOAT2(1, 0) },
	};

	UINT floorIndices[] = { 0, 2, 1, 3, 0, 1 };
	Entity* floor = new Entity();
	floor->AddQuad(floorVertices, floorIndices);
	floor->Finalize();
	floor->transform.Translate(XMFLOAT3(-5, 5, 0));
	entities.push_back(floor);
	AssetManager::Instance()->StoreMaterial(new Material(XMFLOAT4(0.3f, 0.3f, 0.3f, 1), XMFLOAT4(0.0f, 0.2f, 1, 1), XMFLOAT4(1, 1, 1, 1), 16), "floor");
	floor->SetMaterial("floor");
	
	camera->transform.SetParent(&player->transform);
	player->transform.Translate(XMFLOAT3(1, 0, 0));
	XMFLOAT3 eye = camera->transform.GetTranslation();
	XMStoreFloat3(&eye, XMLoadFloat3(&camera->transform.GetTranslation()) + (5 * XMLoadFloat3(&player->transform.GetUp())));
	XMFLOAT3 target;
	XMStoreFloat3(&target, XMLoadFloat3(&player->transform.GetTranslation()) + (3 * XMLoadFloat3(&player->transform.GetForward())));
	XMFLOAT3 up = player->transform.GetUp();
	camera->LookAt(eye, target, up);
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
	assetManager->Instance()->GetSoundManager()->LoadSound(id, L"../Assets/SampleBG.wav");
	id = SoundId::SINK;
	assetManager->Instance()->GetSoundManager()->LoadSound(id, L"../Assets/Sunk.wav");
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
	/*DXGame::OnFocus(givenFocus);
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
	}*/
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
			100.0f);



	/*XMStoreFloat4x4(&camera->projection, XMMatrixTranspose(P));

	if (mouseLook)
	{
		mouseLook->ResetCursor();
	}*/
}
#pragma endregion

#pragma region Game Loop

// Updates the local constant buffer and 
// push it to the buffer on the device
XMFLOAT3 trans = XMFLOAT3(0, 0, 0);
bool scaleSmall = true;
void DemoGame::UpdateScene(float dt)
{
	if (GetAsyncKeyState(VK_ESCAPE))
	{
		PostQuitMessage(0);
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
		//mouseLook->XMove(xnew);
		//entities[1]->transform->Rotate(XMFLOAT3(0, 5 * dt, 0));	
	}

	camera->Update(dt, &vsModelConstantBufferData);	

	if(currentState == GameState::Started)
	{
		GameState newState = menu->Update(dt);
		if (currentState != newState)
		{
			//ShowCursor(false);
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
	deviceContext->ClearRenderTargetView(
		renderTargetView,
		clearColor);

	deviceContext->ClearDepthStencilView(
		depthStencilView, 
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);

	if(currentState == GameState::Started)
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

#ifndef OPTIMIZATION
	deviceContext->IASetInputLayout(inputLayout);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	deviceContext->VSSetConstantBuffers(
		0,	
		1, 
		&vsModelConstantBuffer);
#endif
	if (currentState == GameState::Playing) {		
		/*if(mouseCursorVisibility)
		{
			mouseCursorVisibility = false;
			ShowCursor(mouseCursorVisibility);
		}*/
		// Update light constant buffer for vertex and pixel shader.
		/*materialsAndLightsConstantBufferData.light = light->GetShaderLight();
		materialsAndLightsConstantBufferData.material = AssetManager::Instance()->GetMaterial("default")->GetShaderMaterial();
		DXConnection::Instance()->deviceContext->UpdateSubresource(materialsAndLightsConstantBuffer, 0, NULL, &materialsAndLightsConstantBufferData, 0, 0);
		DXConnection::Instance()->deviceContext->VSSetConstantBuffers(1, 1, &materialsAndLightsConstantBuffer);
		DXConnection::Instance()->deviceContext->PSSetConstantBuffers(1, 1, &materialsAndLightsConstantBuffer);
		*/

		for(Entity* e :entities) 
		{
			// Compute the inverse transpose of the entity's world matrix for use by normals in the shaders. Ignore translation.
			// If the entity is scaled uniformly, cheat and use the world matrix because scales will work.
			XMFLOAT3X3 rotationScale;
			XMStoreFloat3x3(&rotationScale, XMLoadFloat4x4(&e->transform.GetWorldMatrix()));
			XMFLOAT4X4 inverseTranspose;
			if (e->transform.IsUniformScale())
			{
				XMStoreFloat4x4(&inverseTranspose, XMLoadFloat3x3(&rotationScale));
			}
			else
			{
				XMStoreFloat4x4(&inverseTranspose, XMMatrixTranspose(XMMatrixInverse(nullptr, XMLoadFloat3x3(&rotationScale))));
			}
			

			// Create per primitive vertex shader constant buffer to hold matrices.
			VertexShaderModelConstantBuffer perPrimitiveVSConstantBuffer;
			perPrimitiveVSConstantBuffer.world = e->transform.GetWorldMatrix();
			perPrimitiveVSConstantBuffer.inverseTranspose = inverseTranspose;
			perPrimitiveVSConstantBuffer.view = vsModelConstantBufferData.view;
			perPrimitiveVSConstantBuffer.projection = vsModelConstantBufferData.projection;

			// Update vertex shader constant buffer with per primitive buffer.
			DXConnection::Instance()->deviceContext->UpdateSubresource(vsModelConstantBuffer, 0, nullptr, &perPrimitiveVSConstantBuffer, 0, 0);

			// Create per primitive pixel shader constant buffer to hold materials.
			MaterialsAndLightsConstantBuffer perPrimitiveMaterialConstantBuffer;
			perPrimitiveMaterialConstantBuffer.light = materialsAndLightsConstantBufferData.light;
			perPrimitiveMaterialConstantBuffer.material = e->GetMaterial()->GetShaderMaterial();

			// Update pixel shader constant buffer with per primitive materials buffer.
			DXConnection::Instance()->deviceContext->UpdateSubresource(materialsAndLightsConstantBuffer, 0, nullptr, &perPrimitiveMaterialConstantBuffer, 0, 0);
			
			e->Draw();
		}
	}
	flag = true;

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
}

void DemoGame::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}


void DemoGame::OnMouseMove(WPARAM btnState, int x, int y)
{
	if(flag)
	{
		flag = false;
		prevMousePos.x = x;
		prevMousePos.y = y;
		if(currentState == Playing)
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