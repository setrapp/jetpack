#pragma once 
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
	
	inline DirectX::SpriteBatch* SpriteRenderer::GetSpriteBatch() const
	{
		return this->spriteBatch;
	}

	inline void SpriteRenderer::Begin()
	{
		// Store states that about to change.
		deviceContext->OMGetBlendState(&blendState, blendVector, &blendMask);
		deviceContext->OMGetDepthStencilState(&depthStencilState, &pStencil);
		deviceContext->RSGetState(&rasterizerState);

		this->spriteBatch->Begin(DirectX::SpriteSortMode::SpriteSortMode_FrontToBack, nullptr,nullptr,nullptr,nullptr,nullptr, DirectX::XMMatrixIdentity());
	}

	inline void SpriteRenderer::End()
	{
		this->spriteBatch->End();

		// Reset states to what they were before begin.
		deviceContext->OMSetBlendState(blendState, blendVector, blendMask);
		deviceContext->OMSetDepthStencilState(depthStencilState, pStencil);
		deviceContext->RSSetState(rasterizerState);
	}

private: 
	DirectX::SpriteBatch* spriteBatch;
	ID3D11DeviceContext* deviceContext;
	ID3D11BlendState* blendState;
	FLOAT blendVector[4];
	UINT blendMask;
	ID3D11DepthStencilState* depthStencilState;
	UINT pStencil;
	ID3D11RasterizerState* rasterizerState;
};