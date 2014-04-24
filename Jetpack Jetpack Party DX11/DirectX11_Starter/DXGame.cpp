// -------------------------------------------------------------
//  Base class for a DirectX11 "game", heavily borrowing from
//  the structure set up by Frank D. Luna in "3D Game
//  Programming with DirectX 11" (d3dApp.h & d3dApp.cpp)
//
//  This version doesn't rely on D3DX or the Effect framework,
//  as these libraries are deprecated.
// -------------------------------------------------------------

#include "DXGame.h"
#include <WindowsX.h>
#include <sstream>
#include "Toolkit\Inc\CommonStates.h"
#pragma region Global Window Callback
bool DXGame::sysEvent  = false;
namespace
{
	// Allows us to forward Windows messages from a global
	// window procedure to our member function window procedure
	// because we cannot assign a member function to WNDCLASS::lpfnWndProc
	DXGame* dxGame = 0;
	
}
// Set up a global callback for handling windows messages
LRESULT CALLBACK
	MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Forward the global callback to our game's message handling

	// Forward hwnd because we can get messages (e.g., WM_CREATE)
	// before CreateWindow returns, and thus before mhMainWnd is valid.
	return dxGame->MsgProc(hwnd, msg, wParam, lParam);
}
#pragma endregion

#pragma region Constructor / Destructor

DXGame::DXGame(HINSTANCE hInstance)
	:	hAppInst(hInstance),
	windowCaption(L"Jetpack Jetpack Party!"),
	driverType(D3D_DRIVER_TYPE_HARDWARE),
	windowWidth(screenWidth),
	windowHeight(screenHeight),
	enable4xMsaa(false),
	hMainWnd(0),
	gamePaused(false),
	minimized(false),
	maximized(false),
	resizing(false),
	msaa4xQuality(0),
	device(0),
	deviceContext(0),
	swapChain(0),
	depthStencilBuffer(0),
	renderTargetView(0),
	depthStencilView(0)
{
	// Zero out the viewport struct
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
	DXGame::sysEvent =true;
	elapsedTime = 0;
	// Grabs a pointer to this DXGame object so we can forward
	// Windows messages to the object's message handling function
	dxGame = this;
}

DXGame::~DXGame(void)
{
	swapChain->SetFullscreenState(FALSE, NULL);
	// Release the DX stuff
	ReleaseMacro(renderTargetView);
	ReleaseMacro(depthStencilView);
	ReleaseMacro(swapChain);
	ReleaseMacro(depthStencilBuffer);

	// Delete Exposed connection to DirectX
	delete dxConnection;
	dxConnection = 0;

	// Restore default device settings
	if( deviceContext )
		deviceContext->ClearState();

	// Release the context and finally the device
	ReleaseMacro(deviceContext);
	ReleaseMacro(device);
}
#pragma endregion

#pragma region Initialization

// Handles the window and Direct3D initialization
bool DXGame::Init()
{
	if(!InitMainWindow())
		return false;

	if(!InitDirect3D())
		return false;

	initCPU();

	return true;
}

// Initializes the actual window
bool DXGame::InitMainWindow()
{
	// Actually create the window
	WNDCLASS wc;
	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = MainWndProc;  // Can't be a member function!  Hence our global version
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hAppInst;
	wc.hIcon         = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor       = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName  = 0;
	wc.lpszClassName = L"D3DWndClassName";

	if( !RegisterClass(&wc) )
	{
		MessageBox(0, L"RegisterClass Failed.", 0, 0);
		return false;
	}

	// Compute window rectangle dimensions based on requested client area dimensions.
	RECT R = { 0, 0, windowWidth, windowHeight };
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int width  = R.right - R.left;
	int height = R.bottom - R.top;

	hMainWnd = CreateWindow(L"D3DWndClassName", windowCaption.c_str(), 
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, screenWidth, screenHeight, 0, 0, hAppInst, 0); 
	if( !hMainWnd )
	{
		MessageBox(0, L"CreateWindow Failed.", 0, 0);
		return false;
	}

	ShowWindow(hMainWnd, SW_SHOW);
	UpdateWindow(hMainWnd);

	return true;
}

// Create the device, device context, swap chain and necessary buffers
bool DXGame::InitDirect3D()
{
	UINT createDeviceFlags = 0;

	// Do we want a debug device?
#if defined(DEBUG) || defined(_DEBUG)
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// Create the device and determine the supported feature level
	featureLevel = D3D_FEATURE_LEVEL_11_0; // Will be overwritten by next line
	HRESULT hr = D3D11CreateDevice(
		0,
		driverType,
		0,
		createDeviceFlags,
		0,
		0,
		D3D11_SDK_VERSION,
		&device,
		&featureLevel,
		&deviceContext);

	// Handle any device creation or DirectX version errors
	if( FAILED(hr) )
	{
		MessageBox(0, L"D3D11CreateDevice Failed", 0, 0);
		return false;
	}

	// Check for 4X MSAA quality support
	HR(device->CheckMultisampleQualityLevels(
		DXGI_FORMAT_R8G8B8A8_UNORM,
		4,
		&msaa4xQuality));
	assert( msaa4xQuality > 0 ); // Potential problem if quality is 0

	// Set up a swap chain description
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	swapChainDesc.BufferDesc.Width		= windowWidth;
	swapChainDesc.BufferDesc.Height		= windowHeight;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferDesc.Format		= DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling	= DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.BufferUsage			= DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount			= 1;
	swapChainDesc.OutputWindow			= hMainWnd;
	swapChainDesc.Windowed				= true;
	swapChainDesc.SwapEffect			= DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags					= DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	if( enable4xMsaa )
	{
		// Set up 4x MSAA
		swapChainDesc.SampleDesc.Count   = 4;
		swapChainDesc.SampleDesc.Quality = msaa4xQuality - 1;
	}
	else
	{
		// No MSAA
		swapChainDesc.SampleDesc.Count   = 1;
		swapChainDesc.SampleDesc.Quality = 0;
	}

	// To correctly create the swap chain, we must use the IDXGIFactory that
	// was used to create the device.
	IDXGIDevice*	dxgiDevice	= 0;
	IDXGIAdapter*	dxgiAdapter = 0;
	IDXGIFactory*	dxgiFactory = 0;
	HR(device->QueryInterface(	__uuidof(IDXGIDevice),	(void**)&dxgiDevice));
	HR(dxgiDevice->GetParent(	__uuidof(IDXGIAdapter), (void**)&dxgiAdapter));
	HR(dxgiAdapter->GetParent(	__uuidof(IDXGIFactory), (void**)&dxgiFactory));

	// Finally make the swap chain and release the DXGI stuff
	HR(dxgiFactory->CreateSwapChain(device, &swapChainDesc, &swapChain));
	ReleaseMacro(dxgiDevice);
	ReleaseMacro(dxgiAdapter);
	ReleaseMacro(dxgiFactory);

	// Expose widely used DirectX elements
	dxConnection = new DXConnection();
	dxConnection->device = device;
	dxConnection->deviceContext = deviceContext;

	// The remaining steps also need to happen each time the window
	// is resized, so just run the OnResize method
	OnResize();
	return true;
}
#pragma endregion

#pragma region Window Resizing

// Calculates the current aspect ratio
float DXGame::AspectRatio() const
{
	return (float)windowWidth / windowHeight;
}

void DXGame::OnFocus(bool givenFocus) {}

// When the window is resized, the underlying buffers (textures) must
// also be resized to match.  
void DXGame::OnResize()
{
	// Release the views, since we'll be destroying
	// the corresponding buffers.
	ReleaseMacro(renderTargetView);
	ReleaseMacro(depthStencilView);
	ReleaseMacro(depthStencilBuffer);

	// Resize the swap chain to match the window and
	// recreate the render target view
	HR(swapChain->ResizeBuffers(
		1, 
		windowWidth, 
		windowHeight, 
		DXGI_FORMAT_R8G8B8A8_UNORM,
		0));
	ID3D11Texture2D* backBuffer;
	HR(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer)));
	HR(device->CreateRenderTargetView(backBuffer, 0, &renderTargetView));
	ReleaseMacro(backBuffer);

	// Set up the description of the texture to use for the
	// depth stencil buffer
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width			= windowWidth;
	depthStencilDesc.Height			= windowHeight;
	depthStencilDesc.MipLevels		= 1;
	depthStencilDesc.ArraySize		= 1;
	depthStencilDesc.Format			= DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.Usage          = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags      = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0; 
	depthStencilDesc.MiscFlags      = 0;
	if( enable4xMsaa )
	{
		depthStencilDesc.SampleDesc.Count	= 4;
		depthStencilDesc.SampleDesc.Quality = msaa4xQuality - 1;
	}
	else
	{
		depthStencilDesc.SampleDesc.Count   = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
	}

	D3D11_SUBRESOURCE_DATA subData;
	

	// Create the depth/stencil buffer and corresponding view
	//device->CreateTexture2D(&depthStencilDesc, &subData, &depthStencilBuffer);
	HR (device->CreateTexture2D(&depthStencilDesc, 0, &depthStencilBuffer));
	HR (device->CreateDepthStencilView(depthStencilBuffer, 0, &depthStencilView));

	// Bind these views to the pipeline, so rendering actually
	// uses the underlying textures
	deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);

	// Update the viewport and set it on the device
	viewport.TopLeftX	= 0;
	viewport.TopLeftY	= 0;
	viewport.Width		= (float)windowWidth;
	viewport.Height		= (float)windowHeight;
	viewport.MinDepth	= 0.0f;
	viewport.MaxDepth	= 1.0f;
	deviceContext->RSSetViewports(1, &viewport);
}
#pragma endregion

#pragma region Game Loop

// The actual game loop, which processes the windows message queue
// and calls our Update & Draw methods
int DXGame::Run()
{
	MSG msg = {0};
	timer.Reset();

	// Loop until we get a quit message from windows
	while(msg.message != WM_QUIT)
	{
		// Peek at the next message (and remove it from the queue)
		if(PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			// Handle this message
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else
		{
			// No message, so continue the game loop
			timer.Tick();

			if( gamePaused )
			{
				Sleep(100);
			}
			else
			{
				// Standard game loop type stuff
				CalculateFrameStats();
				UpdateScene(timer.DeltaTime());
				//DrawScene();
				elapsedTime += (LONG64)(timer.DeltaTime() * 10000);
                if(((__int64)elapsedTime > 60))
                {
                    elapsedTime %= 60;
                    FixedUpdate();
                }

			}
		}
	}

	return (int)msg.wParam;
}

// Computes the average frames per second, and also the 
// average time it takes to render one frame.  These stats 
// are appended to the window caption bar.
void DXGame::CalculateFrameStats()
{
	static int frameCnt = 0;
	static float timeElapsed = 0.0f;

	frameCnt++;

	// Compute averages over one second period.
	if( (timer.TotalTime() - timeElapsed) >= 1.0f )
	{
		float fps = (float)frameCnt; // fps = frameCnt / 1
		float mspf = 1000.0f / fps;

		std::wostringstream outs;   
		outs.precision(6);
		 memInfo.dwLength = sizeof(MEMORYSTATUSEX);
		GlobalMemoryStatusEx(&memInfo);
		DWORDLONG totalVirtualMem = memInfo.ullTotalPageFile;
		DWORDLONG totalPhysMem = memInfo.dwMemoryLoad;

		outs << windowCaption << L"    "
			<< L"Width: " << windowWidth << L"    "
			<< L"Height: " << windowHeight << L"    "
			<< L"FPS: " << fps << L"    " 
			<< L"Frame Time: " << mspf << L" (ms)"<< "    "
			<< L"CPU : " << getCurrentValue() << "    "
			<< L"RAM : " << totalPhysMem << " MB   ";

		// Include feature level
		switch(featureLevel)
		{
		case D3D_FEATURE_LEVEL_11_1: outs << "    DX 11.1"; break;
		case D3D_FEATURE_LEVEL_11_0: outs << "    DX 11.0"; break;
		case D3D_FEATURE_LEVEL_10_1: outs << "    DX 10.1"; break;
		case D3D_FEATURE_LEVEL_10_0: outs << "    DX 10.0"; break;
		case D3D_FEATURE_LEVEL_9_3:  outs << "    DX 9.3";  break;
		case D3D_FEATURE_LEVEL_9_2:  outs << "    DX 9.2";  break;
		case D3D_FEATURE_LEVEL_9_1:  outs << "    DX 9.1";  break;
		default:                     outs << "    DX ???";  break;
		}

		SetWindowText(hMainWnd, outs.str().c_str());

		// Reset for next average.
		frameCnt = 0;
		timeElapsed += 1.0f;
	}
}

#pragma endregion

#pragma region Windows Message Processing

LRESULT DXGame::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch( msg )
	{
		// WM_ACTIVATE is sent when the window is activated or deactivated.  
		// We pause the game when the window is deactivated and unpause it 
		// when it becomes active.  
	case WM_ACTIVATE:
		if( LOWORD(wParam) == WA_INACTIVE )
		{
			gamePaused = true;
			timer.Stop();
		}
		else
		{
			gamePaused = false;
			timer.Start();
		}
		return 0;
		// WM_SETFOCUS is sent when the window recieves keyboard focus.
	case WM_SETFOCUS:
		OnFocus(true);
		return 0;
	case WM_KILLFOCUS:
		OnFocus(false);
		return 0;
		// WM_SIZE is sent when the user resizes the window.  
	case WM_SIZE:
		// Save the new client area dimensions.
		windowWidth  = LOWORD(lParam);
		windowHeight = HIWORD(lParam);
		if( device )
		{
			if( wParam == SIZE_MINIMIZED )
			{
				gamePaused = true;
				minimized = true;
				maximized = false;
			}
			else if( wParam == SIZE_MAXIMIZED )
			{
				gamePaused = false;
				minimized = false;
				maximized = true;
				OnResize();
			}
			else if( wParam == SIZE_RESTORED )
			{
				// Restoring from minimized state?
				if( minimized )
				{
					gamePaused = false;
					minimized = false;
					OnResize();
				}

				// Restoring from maximized state?
				else if( maximized )
				{
					gamePaused = false;
					maximized = false;
					OnResize();
				}
				else if( resizing )
				{
					// If user is dragging the resize bars, we do not resize 
					// the buffers here because as the user continuously 
					// drags the resize bars, a stream of WM_SIZE messages are
					// sent to the window, and it would be pointless (and slow)
					// to resize for each WM_SIZE message received from dragging
					// the resize bars.  So instead, we reset after the user is 
					// done resizing the window and releases the resize bars, which 
					// sends a WM_EXITSIZEMOVE message.
				}
				else // API call such as SetWindowPos or mSwapChain->SetFullscreenState.
				{
					OnResize();
				}
			}
		}
		return 0;

		// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
	case WM_ENTERSIZEMOVE:
		gamePaused = true;
		resizing  = true;
		timer.Stop();
		return 0;

		// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
		// Here we reset everything based on the new window dimensions.
	case WM_EXITSIZEMOVE:
		gamePaused = false;
		resizing  = false;
		timer.Start();
		OnResize();
		return 0;

		// WM_DESTROY is sent when the window is being destroyed.
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

		// The WM_MENUCHAR message is sent when a menu is active and the user presses 
		// a key that does not correspond to any mnemonic or accelerator key. 
	case WM_MENUCHAR:
		// Don't beep when we alt-enter.
		return MAKELRESULT(0, MNC_CLOSE);

		// Catch this message so to prevent the window from becoming too small.
	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200; 
		return 0;

	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
		OnMouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
		OnMouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_MOUSEMOVE:
		OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));		
		return 0;
	case WM_MOUSEWHEEL:
		OnMouseWheel(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}


#include "windows.h"


static ULARGE_INTEGER lastCPU, lastSysCPU, lastUserCPU;
static int numProcessors;
static HANDLE self;


void DXGame::initCPU(){



    SYSTEM_INFO sysInfo;
    FILETIME ftime, fsys, fuser;


    GetSystemInfo(&sysInfo);
    numProcessors = sysInfo.dwNumberOfProcessors;


    GetSystemTimeAsFileTime(&ftime);
    memcpy(&lastCPU, &ftime, sizeof(FILETIME));


    self = GetCurrentProcess();
    GetProcessTimes(self, &ftime, &ftime, &fsys, &fuser);
    memcpy(&lastSysCPU, &fsys, sizeof(FILETIME));
    memcpy(&lastUserCPU, &fuser, sizeof(FILETIME));
}


double DXGame::getCurrentValue(){
    FILETIME ftime, fsys, fuser;
    ULARGE_INTEGER now, sys, user;
    double percent;


    GetSystemTimeAsFileTime(&ftime);
    memcpy(&now, &ftime, sizeof(FILETIME));


    GetProcessTimes(self, &ftime, &ftime, &fsys, &fuser);
    memcpy(&sys, &fsys, sizeof(FILETIME));
    memcpy(&user, &fuser, sizeof(FILETIME));
    percent = (sys.QuadPart - lastSysCPU.QuadPart) +
        (user.QuadPart - lastUserCPU.QuadPart);
    percent /= (now.QuadPart - lastCPU.QuadPart);
    percent /= numProcessors;
    lastCPU = now;
    lastUserCPU = user;
    lastSysCPU = sys;


    return percent * 100;
}

#pragma endregion




