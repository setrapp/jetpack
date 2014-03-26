#ifndef _RENDERER_H
#define _RENDERER_H
#include <d3d11.h>
#include "Toolkit\Inc\SpriteBatch.h"


class Renderer
{
public:
	Renderer::Renderer(void)
	{

	}

	virtual Renderer::~Renderer(void)
	{

	}

	virtual DirectX::SpriteBatch* Renderer::GetSpriteBatch()
	{
		return spriteBatch;
	}

private: DirectX::SpriteBatch* spriteBatch;

};
#endif