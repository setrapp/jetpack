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
#define BUFFERED_STUFF
#include <Windows.h>
#include <d3dcompiler.h>
#include "DemoGame.h"

#include "Common.h"
#include <stdio.h>
#include <stdlib.h>
#include <comdef.h>
#include <iostream>
#include "ModelLoad\MLModelViewer.h"
#include "Player.h"
#include "GreaterMesh.h"

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
	light = new Light(XMFLOAT3(0, 1, -1), XMFLOAT4(0.2f, 0.2f, 0.2f, 1), XMFLOAT4(1, 1, 1, 1), false);
	soundManager = new SoundManager();
}

DemoGame::~DemoGame()
{
	ReleaseMacro(vertexShader);
	ReleaseMacro(pixelShader);
	ReleaseMacro(vsModelConstantBuffer);
	ReleaseMacro(vsFrameConstantBuffer);
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
		{ XMFLOAT3(+1.0f, +1.0f, +0.0f), XMFLOAT3(0, 0, 1), red, XMFLOAT2(0, 0) },
		{ XMFLOAT3(-1.0f, -1.0f, +0.0f), XMFLOAT3(0, 0, 1), green, XMFLOAT2(1, 1) },
		{ XMFLOAT3(+1.0f, -1.0f, +0.0f), XMFLOAT3(0, 0, 1), blue, XMFLOAT2(0, 1) },		
		{ XMFLOAT3(-1.0f, +1.0f, +0.0f), XMFLOAT3(0, 0, 1), mid, XMFLOAT2(1, 0) },
	};

	UINT indices[] = { 0, 2, 1, 3, 0, 1 };	

	Vertex floorVertices[] = 
	{
		{ XMFLOAT3(40.0f, -3.0f, 40.0f), XMFLOAT3(0, 1, 0), red, XMFLOAT2(0, 0) },
		{ XMFLOAT3(-40.0f, -3.0f, -40.0f), XMFLOAT3(0, 1, 0), green, XMFLOAT2(1, 1) },
		{ XMFLOAT3(40.0f, -3.0f, -40.0f), XMFLOAT3(0, 1, 0), blue, XMFLOAT2(0, 1) },		
		{ XMFLOAT3(-40.0f, -3.0f, 40.0f), XMFLOAT3(0, 1, 0), mid, XMFLOAT2(1, 0) },
	};

	UINT floorIndices[] = { 0, 2, 1, 3, 0, 1 };	

	Entity* floor = new Entity();
	floor->AddQuad(floorVertices, floorIndices);
	floor->Finalize();
	entities.push_back(floor);
	
	/*for(int i = 0 ; i < 5; i ++)
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
		//entity->AddQuad(vertices, indices);
		//if(rand() % 10 < 5)
		//	entity->LoadTexture(L"../Assets/RedGift.png");
		//entities.push_back(entity);
	}*/

	// Attempt to load model
	MLModel3D* model = mlModel3DLoadOBJ("../Assets/video_camera.obj");
	Player* modelEnt = new Player();
	bool hasUVs = mlModel3DGetTextureVertexCount(model) > 1;
	unsigned int faceCount = mlModel3DGetFaceCount(model);
	for (int i = 0; i < faceCount; i++) {
		// Retrieve current face.
		MLFace3D const* face = mlModel3DGetFace(model, i);

		// Retrieve vertices that make up current face.
		unsigned short mlIndex;
		MLVertex3D const* mlVertex;
		MLTexelXY const* mlTexel;
		GUPoint3D guPoint;
		GUNormal3D guNormal;// = mlVertex3DGetNormal(mlVertex);
		GUPoint2D guUV;
		// Vertex 1
		mlIndex = mlFace3DGetVertex1(face);
		mlVertex = mlModel3DGetVertex(model, mlIndex);
		guPoint = mlVertex3DGetPosition(mlVertex);
		guNormal = mlVertex3DGetNormal(mlVertex);
		Vertex vertex1;
		vertex1.Position = XMFLOAT3(guPoint.x, guPoint.y, guPoint.z);
		vertex1.Normal = XMFLOAT3(guNormal.x, guNormal.y, guNormal.z);
		vertex1.Color = red;
		if (hasUVs) {
			MLTexelXY const* mlTexel = mlModel3DGetTextureVertex(model, mlIndex);
			GUPoint2D guUV = mlTexelXYGetPosition(mlTexel);
			vertex1.UV = XMFLOAT2(guUV.x, guUV.y);
		} else {
			vertex1.UV = XMFLOAT2(0, 0);
		}
		// Vertex 2
		mlIndex = mlFace3DGetVertex2(face);
		mlVertex = mlModel3DGetVertex(model, mlIndex);
		guPoint = mlVertex3DGetPosition(mlVertex);
		guNormal = mlVertex3DGetNormal(mlVertex);
		Vertex vertex2;
		vertex2.Position = XMFLOAT3(guPoint.x, guPoint.y, guPoint.z);
		vertex2.Normal = XMFLOAT3(guNormal.x, guNormal.y, guNormal.z);
		vertex2.Color = red;
		if (hasUVs) {
			MLTexelXY const* mlTexel = mlModel3DGetTextureVertex(model, mlIndex);
			GUPoint2D guUV = mlTexelXYGetPosition(mlTexel);
			vertex2.UV = XMFLOAT2(guUV.x, guUV.y);
		} else {
			vertex2.UV = XMFLOAT2(0, 0);
		}
		// Vertex 3
		mlIndex = mlFace3DGetVertex3(face);
		mlVertex = mlModel3DGetVertex(model, mlIndex);
		guPoint = mlVertex3DGetPosition(mlVertex);
		guNormal = mlVertex3DGetNormal(mlVertex);
		Vertex vertex3;
		vertex3.Position = XMFLOAT3(guPoint.x, guPoint.y, guPoint.z);
		vertex3.Normal = XMFLOAT3(guNormal.x, guNormal.y, guNormal.z);
		vertex3.Color = red;
		vertex3.UV = XMFLOAT2(0,0);
		if (hasUVs) {
			MLTexelXY const* mlTexel = mlModel3DGetTextureVertex(model, mlIndex);
			GUPoint2D guUV = mlTexelXYGetPosition(mlTexel);
			vertex3.UV = XMFLOAT2(guUV.x, guUV.y);
		} else {
			vertex3.UV = XMFLOAT2(0, 0);
		}

		// Create usable mesh.
		Vertex vertices[] = {vertex1, vertex2, vertex3};
		UINT indices[] = {0, 1, 2};
		modelEnt->AddTriangle(vertices, indices);
		modelEnt->camera = camera;
		modelEnt->cameraPos = XMFLOAT3(0, 0, 100);
	}
	//modelEnt->LoadTexture(L"../Assets/RedGift.png");
	modelEnt->Finalize();
	entities.push_back(modelEnt);


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
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,	0, 0,								D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	// Load Vertex Shader --------------------------------------
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

	// Vertex Shader Per Frame Constant Buffer
	D3D11_BUFFER_DESC cBufferDesc2;
	cBufferDesc2.ByteWidth			= sizeof(vsFrameConstantBufferData);
	cBufferDesc2.Usage				= D3D11_USAGE_DEFAULT;
	cBufferDesc2.BindFlags			= D3D11_BIND_CONSTANT_BUFFER;
	cBufferDesc2.CPUAccessFlags		= 0;
	cBufferDesc2.MiscFlags			= 0;
	cBufferDesc2.StructureByteStride = 0;
	HR(device->CreateBuffer(
		&cBufferDesc2,
		NULL,
		&vsFrameConstantBuffer));

	
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
	
	if(currentState == GameState::Playing)
	{
	this->deltaTime = dt;
	
	//entity.Rotate(XMFLOAT3(0, 0, x));
	//dt *= 5;

	
	for(Entity* e: entities)
		{
			// Reset entity translation back to origin. This is not need, just preserving the jitter effect.
			//XMFLOAT4X4 eTrans = e->transform->trans;
			//e->transform->Translate(XMFLOAT3(-eTrans._41, -eTrans._42, -eTrans._43));

			//auto p = rand() % 2;
			//p++;
			/*if(p <4 )
				e->transform->Translate(XMFLOAT3(rand() % p * 0.1f, rand() % p * 0.1f, rand() % p * 0.1f));
			else
				e->transform->Rotate(XMFLOAT3(rand() % p * 0.1f, rand() % p * 0.1f, rand() % p * 0.1f));
			
			// Move entity away the screen a bit
			e->transform->Translate(XMFLOAT3(0.0f, 0.0f, 5.0f));*/
			
			//e->transform->Rotate(XMFLOAT3(0, 0.001f, 0));

			/*} else {
				e->transform->Scale(XMFLOAT3(1.11111f, 1.11111f, 1.11111f));
			}
			scaleSmall = !scaleSmall;*/

			e->Update(dt);
		}
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

	deviceContext->VSSetShader(
		vertexShader, 
		NULL, 
		0);

	deviceContext->VSSetConstantBuffers(
		0,	
		1, 
		&vsModelConstantBuffer);

	deviceContext->PSSetShader(
		pixelShader, 
		NULL, 
		0);
#endif
	if (currentState == GameState::Playing) {
		vsFrameConstantBufferData.light = light->ConvertToShaderLight();
		DXConnection::Instance()->deviceContext->UpdateSubresource(vsFrameConstantBuffer, 0, NULL, &vsFrameConstantBufferData, 0, 0);
		DXConnection::Instance()->deviceContext->VSSetConstantBuffers(1, 1, &vsFrameConstantBuffer);

		// TODO: Model Vertices are not being rendered at correct depth, is this a problem with depth buffer or model???

		for(Entity* e :entities) 
		{
			// Create per primitive vertex shader constant buffer to hold world matrix.
			VertexShaderModelConstantBuffer perPrimitiveVSConstantBuffer;
			perPrimitiveVSConstantBuffer.world = e->transform->worldMatrix;
			perPrimitiveVSConstantBuffer.view = vsModelConstantBufferData.view;
			perPrimitiveVSConstantBuffer.projection = vsModelConstantBufferData.projection;
			
			// Update vertex shader constant buffer with per primitive buffer.
			DXConnection::Instance()->deviceContext->UpdateSubresource(vsModelConstantBuffer, 0, nullptr, &perPrimitiveVSConstantBuffer, 0, 0);
			
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