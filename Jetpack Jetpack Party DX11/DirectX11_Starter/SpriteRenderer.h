
#include <d3d11.h>
#include "Toolkit\Inc\SpriteBatch.h"
#include "Renderer.h"

class SpriteRenderer: Renderer
{
public:
	SpriteRenderer::SpriteRenderer(ID3D11DeviceContext* deviceContext)
	{
		this->deviceContext = deviceContext;
		spriteBatch = new DirectX::SpriteBatch(deviceContext);
	}

	~SpriteRenderer(void)
	{
		delete spriteBatch;
	}

	inline void SpriteRenderer::SetColor(FLOAT* color)
	{
		this->COLOR = color;
	}

	void SpriteRenderer::ClearScreen(ID3D11DeviceContext* deviceContext, ID3D11RenderTargetView* renderTargetView, ID3D11DepthStencilView* depthStencilView)
	{
	deviceContext->ClearRenderTargetView(
		renderTargetView, 
		COLOR);

	deviceContext->ClearDepthStencilView(
		depthStencilView, 
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);
	};

	inline DirectX::SpriteBatch* SpriteRenderer::GetSpriteBatch() const
	{
		return this->spriteBatch;
	}

	inline void SpriteRenderer::Begin()
	{
		// Store states that about to change.
		deviceContext->OMGetBlendState(&blendState, blendVector, &blendMask);
		deviceContext->OMGetDepthStencilState(&depthStencilState, NULL);
		deviceContext->RSGetState(&rasterizerState);

		this->spriteBatch->Begin(DirectX::SpriteSortMode::SpriteSortMode_FrontToBack, nullptr,nullptr,nullptr,nullptr,nullptr, DirectX::XMMatrixIdentity());
	}

	inline void SpriteRenderer::End()
	{
		this->spriteBatch->End();

		// Reset states to what they were before begin.
		deviceContext->OMSetBlendState(blendState, blendVector, blendMask);
		deviceContext->OMSetDepthStencilState(depthStencilState, NULL);
		deviceContext->RSSetState(rasterizerState);
	}

private: 
	FLOAT* COLOR;
	DirectX::SpriteBatch* spriteBatch;
	ID3D11DeviceContext* deviceContext;
	ID3D11BlendState* blendState;
	FLOAT blendVector[4];
	UINT blendMask;
	ID3D11DepthStencilState* depthStencilState;
	ID3D11RasterizerState* rasterizerState;
};