#include "DeferredRenderer.h"
#include "dxerr.h"

DeferredRenderer::DeferredRenderer(int textureWidth, int textureHeight)
{
	device = DXConnection::Instance()->device;
	deviceContext = DXConnection::Instance()->deviceContext;

	// Initialize everything to NULL.
	for (int i = 0; i < TARGET_COUNT; i++)
	{
		targetTextures[i] = NULL;
		targetViews[i] = NULL;
		shaderResourceViews[i] = NULL;
	}
	depthStencilBuffer = NULL;
	depthStencilView = NULL;

	// Setup render target textures.
	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(textureDesc));
	textureDesc.Width = textureWidth;
	textureDesc.Height = textureHeight;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	for (int i = 0; i < TARGET_COUNT; i++)
	{
		device->CreateTexture2D(&textureDesc, NULL, &targetTextures[i]);
	}

	// Setup render target views.
	D3D11_RENDER_TARGET_VIEW_DESC targetViewDesc;
	ZeroMemory(&targetViewDesc, sizeof(targetViewDesc));
	targetViewDesc.Format = textureDesc.Format;
	targetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	for (int i = 0; i < TARGET_COUNT; i++)
	{
		device->CreateRenderTargetView(targetTextures[i], &targetViewDesc, &targetViews[i]);
	}

	// Setup shader resource views.
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	ZeroMemory(&shaderResourceViewDesc, sizeof(shaderResourceViewDesc));
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;
	for (int i = 0; i < TARGET_COUNT; i++)
	{
		device->CreateShaderResourceView(targetTextures[i], &shaderResourceViewDesc, &shaderResourceViews[i]);
	}

	// Setup depth stencil buffer.
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
	depthBufferDesc.Width = textureWidth;
	depthBufferDesc.Height = textureHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	HRESULT hr = device->CreateTexture2D(&depthBufferDesc, NULL, &depthStencilBuffer);
	if (FAILED(hr))
	{
		int a = 0;
	}

	// Setup depth stencil view.
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	hr = device->CreateDepthStencilView(depthStencilBuffer, &depthStencilViewDesc, &depthStencilView);
	if (FAILED(hr))
	{
		int a = 0;
	}

	// Setup viewport.
	viewport.Width = textureWidth;
	viewport.Height = textureHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
}

DeferredRenderer::~DeferredRenderer()
{
	if (depthStencilView)
	{
		ReleaseMacro(depthStencilView);
	}
	if (depthStencilBuffer)
	{
		ReleaseMacro(depthStencilBuffer);
	}
	for (int i = 0; i < TARGET_COUNT; i++)
	{
		if (shaderResourceViews[i])
		{
			ReleaseMacro(shaderResourceViews[i]);
		}
		if (targetViews[i])
		{
			ReleaseMacro(targetViews[i]);
		}
		if (targetTextures[i])
		{
			ReleaseMacro(targetTextures[i]);
		}
	}
}

void DeferredRenderer::SetTargets()
{
	deviceContext->OMSetRenderTargets(TARGET_COUNT, targetViews, depthStencilView);
	deviceContext->RSSetViewports(1, &viewport);
}

void DeferredRenderer::ClearTargets(float clearColor[4])
{
	for (int i = 0; i < TARGET_COUNT; i++)
	{
		deviceContext->ClearRenderTargetView(targetViews[i], clearColor);
	}
	deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

ID3D11ShaderResourceView* DeferredRenderer::GetShaderResourceView(int index)
{
	return shaderResourceViews[index];
}