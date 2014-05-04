#pragma once 
#include <d3d11.h>
#include "Toolkit\Inc\SpriteBatch.h"
#include "Renderer.h"
#include "Toolkit\Inc\CommonStates.h"
#include "DXConnection.h"


using namespace DirectX;
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
		deviceContext->OMGetDepthStencilState(&depthStencilState, NULL);
		deviceContext->RSGetState(&rasterizerState);
		
		CommonStates states(DXConnection::Instance()->device);
		deviceContext->OMSetBlendState(states.NonPremultiplied(), nullptr, 0xFFFFFFFF);
		spriteBatch->Begin(SpriteSortMode_BackToFront, nullptr, nullptr, nullptr, nullptr, [=]
		{			
			CommonStates states(DXConnection::Instance()->device);
			deviceContext->OMSetBlendState( states.NonPremultiplied(), nullptr, 0xFFFFFFFF);
		});
	}

	inline void SpriteRenderer::End()
	{
		this->spriteBatch->End();

		// Reset states to what they were before begin.
		deviceContext->OMSetBlendState(blendState, blendVector, blendMask);
		deviceContext->OMSetDepthStencilState(depthStencilState, NULL);
		deviceContext->RSSetState(rasterizerState);
		if (blendState)
		{
			blendState->Release();
		}
		if (depthStencilState)
		{
			depthStencilState->Release();
		}
		if (rasterizerState)
		{
			rasterizerState->Release();
		}
	}

private: 
	DirectX::SpriteBatch* spriteBatch;
	ID3D11DeviceContext* deviceContext;
	ID3D11BlendState* blendState;
	FLOAT blendVector[4];
	UINT blendMask;
	ID3D11DepthStencilState* depthStencilState;
	ID3D11RasterizerState* rasterizerState;
};