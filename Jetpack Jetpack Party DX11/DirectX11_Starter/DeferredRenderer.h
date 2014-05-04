#pragma once
#include "Common.h"

#define TARGET_COUNT 4

class DeferredRenderer
{
public:
	DeferredRenderer(int textureWidth, int textureHeight);
	~DeferredRenderer();
	void SetTargets();
	void ClearTargets(float clearColor[4]);
	ID3D11ShaderResourceView** GetShaderResourceViews();
	ID3D11ShaderResourceView* GetShaderResourceView(int index);
private:
	ID3D11Texture2D* targetTextures[TARGET_COUNT];
	ID3D11RenderTargetView* targetViews[TARGET_COUNT];
	ID3D11ShaderResourceView* shaderResourceViews[TARGET_COUNT];
	ID3D11Texture2D* depthStencilBuffer;
	ID3D11DepthStencilView* depthStencilView;
	D3D11_VIEWPORT viewport;
	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;
};