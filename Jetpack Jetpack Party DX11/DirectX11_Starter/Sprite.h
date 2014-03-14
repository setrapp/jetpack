
#include <d3d11.h>
#include "Toolkit\Inc\SpriteBatch.h"
#include "Renderer.h"

class SpriteRenderer: Renderer
{
public:
	SpriteRenderer::SpriteRenderer(ID3D11DeviceContext* deviceContext)
	{
		spriteBatch = new DirectX::SpriteBatch(deviceContext);
	}

	~SpriteRenderer(void)
	{
		
	}

	void SpriteRenderer::SetColor(FLOAT* color)
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

	DirectX::SpriteBatch* SpriteRenderer::GetSpriteBatch()
	{
		return this->spriteBatch;
	}

	void SpriteRenderer::Begin()
	{
		this->spriteBatch->Begin(DirectX::SpriteSortMode::SpriteSortMode_FrontToBack, nullptr,nullptr,nullptr,nullptr,nullptr, DirectX::XMMatrixIdentity());
	}

	void SpriteRenderer::End()
	{
		this->spriteBatch->End();
	}

private: 
	FLOAT* COLOR;
	DirectX::SpriteBatch* spriteBatch;
};