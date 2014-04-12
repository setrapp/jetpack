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
#include <comdef.h>
#include <iostream>
#include "Player.h"

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
	windowCaption = L"Jetpack Jetpack Party!";
	windowWidth = 800;
	windowHeight = 600;
	currentState = GameState::Started;
	menu = new Menu(device, deviceContext);
	camera = new ControllableCamera();
	light = new Light(XMFLOAT3(0, -1, 1), XMFLOAT4(1, 1, 1, 1), XMFLOAT4(1, 1, 1, 1), XMFLOAT4(1, 1, 1, 1), true);
	soundManager = new SoundManager();
}

DemoGame::~DemoGame()
{
	ReleaseMacro(vertexShader);
	ReleaseMacro(pixelShader);
	ReleaseMacro(vsModelConstantBuffer);
	ReleaseMacro(materialsAndLightsConstantBuffer);
	ReleaseMacro(inputLayout);

	delete light;
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

	LoadShadersAndInputLayout();

	AssetManager::Instance()->StoreMaterial(new Material());

	XMFLOAT3 cameraPosition;
	XMStoreFloat3(&cameraPosition, XMVectorSet(0, 0, -20, 0));
	XMFLOAT3 cameraTarget;
	XMStoreFloat3(&cameraTarget, XMVectorSet(0, 0, 0, 0));
	XMFLOAT3 cameraUp;
	XMStoreFloat3(&cameraUp, XMVectorSet(0, 1, 0, 0));

	camera->LookAt(cameraPosition, cameraTarget, cameraUp);

	// Set up buffers and such
	CreateGeometryBuffers();
	this->deltaTime = 0;

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

	// Attempt to load model
	AssetManager::Instance()->CreateAndStoreModel("../Assets/video_camera.obj", "camera");
	Player* player = new Player();
	player->AddModel(AssetManager::Instance()->GetModel("camera"));
	entities.push_back(player);
	player->Finalize();
	AssetManager::Instance()->StoreMaterial(new Material(XMFLOAT4(0.3, 0.3, 0.3, 1), XMFLOAT4(1, 0, 1, 1), XMFLOAT4(0.5f, 0.5f, 0.5f, 0.5f), 16), "camera");
	player->SetMaterial("camera");

	Entity* emptyEntity = new Entity();
	entities.push_back(emptyEntity);

	AssetManager::Instance()->CreateAndStoreModel("../Assets/cube.obj", "cube");
	Entity* cube = new Entity();
	cube->AddModel(AssetManager::Instance()->GetModel("cube"));
	cube->Finalize();
	cube->transform->Translate(XMFLOAT3(5, 0, 0));
	entities.push_back(cube);
	cube->transform->SetParent(emptyEntity->transform);
	emptyEntity->transform->SetParent(player->transform);

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
	gift->transform->Translate(XMFLOAT3(-5, 5, 0));
	entities.push_back(gift);
	AssetManager::Instance()->StoreMaterial(new Material(XMFLOAT4(0.3f, 0.3f, 0.3f, 1), XMFLOAT4(1, 1, 1, 1), XMFLOAT4(1, 1, 1, 1), 16), "gift");
	gift->SetMaterial("gift");
	gift->GetMaterial()->pixelShader = AssetManager::Instance()->GetPixelShader("texture");
	gift->LoadTexture(L"../Assets/RedGift.png");

	camera->transform->SetParent(player->transform);
	player->transform->Translate(XMFLOAT3(5, 0, 0));
	//camera->transform->SetLocalTranslation(XMFLOAT3(0, 2, -10));
	//camera->transform->Translate(XMFLOAT3(5, 0, 0));
	//camera->LookAt(camera->transform->GetTranslation(), gift->transform->GetTranslation(), player->transform->GetUp());
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
XMFLOAT3 trans = XMFLOAT3(0, 0, 0);
bool scaleSmall = true;
void DemoGame::UpdateScene(float dt)
{
	soundManager->Update();
	if(currentState == GameState::Playing)
	{
	this->deltaTime = dt;


	for(Entity* e: entities)
		{
			e->Update(dt);
		}

		entities[1]->transform->Rotate(XMFLOAT3(0, 2 * dt, 0));
	}

	camera->Update(dt, &vsModelConstantBufferData);	

	if(currentState == GameState::Started)
	{
		currentState = menu->Update(dt);
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
	
	deviceContext->VSSetConstantBuffers(
		0,	
		1, 
		&vsModelConstantBuffer);
#endif
	if (currentState == GameState::Playing) {

		// Update light constant buffer for vertex and pixel shader.
		materialsAndLightsConstantBufferData.light = light->GetShaderLight();
		materialsAndLightsConstantBufferData.material = AssetManager::Instance()->GetMaterial("default")->GetShaderMaterial();
		DXConnection::Instance()->deviceContext->UpdateSubresource(materialsAndLightsConstantBuffer, 0, NULL, &materialsAndLightsConstantBufferData, 0, 0);
		DXConnection::Instance()->deviceContext->VSSetConstantBuffers(1, 1, &materialsAndLightsConstantBuffer);
		DXConnection::Instance()->deviceContext->PSSetConstantBuffers(1, 1, &materialsAndLightsConstantBuffer);
		
		for(Entity* e :entities) 
		{
			// Compute the inverse transpose of the entity's world matrix for use by normals in the shaders. Ignore translation.
			// If the entity is scaled uniformly, cheat and use the world matrix because scales will work.
			XMFLOAT3X3 rotationScale;
			XMStoreFloat3x3(&rotationScale, XMLoadFloat4x4(&e->transform->GetWorldMatrix()));
			XMFLOAT4X4 inverseTranspose;
			if (e->transform->IsUniformScale())
			{
				XMStoreFloat4x4(&inverseTranspose, XMLoadFloat3x3(&rotationScale));
			}
			else
			{
				XMStoreFloat4x4(&inverseTranspose, XMMatrixTranspose(XMMatrixInverse(nullptr, XMLoadFloat3x3(&rotationScale))));
			}
			

			// Create per primitive vertex shader constant buffer to hold matrices.
			VertexShaderModelConstantBuffer perPrimitiveVSConstantBuffer;
			perPrimitiveVSConstantBuffer.world = e->transform->GetWorldMatrix();
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